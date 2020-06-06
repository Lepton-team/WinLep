#include "lepton_converter.h"
#include "process_util.h"

std::vector<BYTE> wlep::LeptonConverter::convertJpgToLepton(const std::string &jpg_filename) {
	const std::string lepton_exe = "lepton.exe";
	const std::string lepton_args = "-skiproundtrip -";
	std::vector<BYTE> lepton_data = std::vector<BYTE>();

	// Pipe handles
	HANDLE child_in_read = NULL;
	HANDLE child_in_write = NULL;
	HANDLE child_out_read = NULL; // Here is the converted lepton image
	HANDLE child_out_write = NULL;

	SECURITY_ATTRIBUTES sec_attr;
	sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
	sec_attr.bInheritHandle = true;
	sec_attr.lpSecurityDescriptor = NULL;

	HANDLE input_file_handle = CreateFileA(jpg_filename.c_str(),
										   GENERIC_READ,
										   0,
										   NULL,
										   OPEN_EXISTING,
										   FILE_ATTRIBUTE_READONLY,
										   NULL
	);

	if (input_file_handle == INVALID_HANDLE_VALUE) {
		std::string msg = "Error while opening file " + jpg_filename + " !";
		wleputils::ExceptionUtil::throwAndPrintException
			<std::exception>(msg);
	}

	wleputils::ProcessUtil::createPipes(child_in_read, child_in_write, child_out_read, child_out_write, sec_attr);
	bool is_process_launched = wleputils::ProcessUtil::launchProcess(lepton_exe, lepton_args, child_out_write, child_in_read);

	if (!is_process_launched) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::exception>("Could not create child process", "CreateProcess failed with an error", GetLastError());
	} else {
		// Close the in pipe's reading end
		if (!CloseHandle(child_in_read)) {
			wleputils::ExceptionUtil::throwAndPrintException
				<std::exception>("Error while closing 'in' pipe's reading end!", GetLastError());
		}

		// Close the out pipe's writing end
		if (!CloseHandle(child_out_write)) {
			wleputils::ExceptionUtil::throwAndPrintException
				<std::exception>("Error while closing 'out' pipe's writing end!", GetLastError());
		}
	}
	// Input file and child_in_write are closed here
	wleputils::ProcessUtil::writeToPipe(input_file_handle, child_in_write);
	wleputils::ProcessUtil::readFromPipeToBuffer(child_out_read, lepton_data);

	// Close the out pipe's writing end
	if (!CloseHandle(child_out_read)) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::exception>("Error while closing 'out' pipe's reading end!", GetLastError());
	}


	return lepton_data;
}

#pragma once

#define DEBUG 

#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <string>
#include "exception_util.h"

namespace wleputils {

	class ProcessUtil {
	public:
		static PROCESS_INFORMATION launchProcessAndWait(const std::string &executable, const std::string &args) {
			STARTUPINFO start_info;
			PROCESS_INFORMATION proc_info; // Return value

			ZeroMemory(&start_info, sizeof(start_info));
			start_info.cb = sizeof(start_info);
			
			//TODO: 
			// 1. Create pipe
			// 2. Create process
			// 3. Read lepton_data
			//start_info.hStdInput = // JPG Input file handle
			//start_info.hStdOutput = // Zapisovaci koniec rury 
			//start_info.dwFlags |= STARTF_USESTDHANDLES;

			ZeroMemory(&proc_info, sizeof(proc_info));

			// Prepare CreateProcess args
			std::wstring exe(executable.length(), L' '); // Make room for characters
			std::copy(executable.begin(), executable.end(), exe.begin()); // Copy string to wstring.

			std::wstring w_args(args.length(), L' '); // Make room for characters
			std::copy(args.begin(), args.end(), w_args.begin()); // Copy string to wstring.

			std::wstring input = exe + L" " + w_args;
			wchar_t *args_concat = const_cast<wchar_t *>(input.c_str());
			const wchar_t *app_exe = exe.c_str();

			// Start the child process.
			// With CreateProcessW there's no need to set the arv[0] as the executable (itself)
			if (!CreateProcessW(
				app_exe,        // app path
				args_concat,    // Command line (needs to include app path as first argument. args seperated by whitepace)
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				TRUE,           // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory
				(LPSTARTUPINFOW)&start_info,  // Pointer to STARTUPINFO structure
				&proc_info)     // Pointer to PROCESS_INFORMATION structure
				) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Could not create child process", "CreateProcess failed with error", GetLastError());
			} else {
#ifdef DEBUG
				std::cout << "[OK] Successfully launched a child process" << std::endl;
#endif // DEBUG
			}

			WaitForSingleObject(proc_info.hProcess, INFINITE);

			return proc_info;
		}

		static bool isProcessActive(PROCESS_INFORMATION &pi) {
			// Check if handle is closed
			if (pi.hProcess == NULL) {
				wleputils::ExceptionUtil::printErrorMsg("Process handle is closed or invalid", GetLastError());
				return false;
			}

			// If handle open is, check if process is active
			DWORD exit_code = 0;
			if (GetExitCodeProcess(pi.hProcess, &exit_code) == 0) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Cannot return exit code", GetLastError());
			} else {
				return exit_code == STILL_ACTIVE;
			}
		}

		static bool stopProcess(PROCESS_INFORMATION &pi) {
			if (!isProcessActive(pi)) {
#ifdef DEBUG
				std::cout << "[OK] Process already inactive.\n";
#endif // DEBUG
				return true;
			}
			
			// Check if handle is invalid or has allready been closed
			if (pi.hProcess == NULL) {
				wleputils::ExceptionUtil::printErrorMsg("Process handle invalid. It was probably already closed", GetLastError());
				return false;
			}

			// Terminate Process
			if (!TerminateProcess(pi.hProcess, 1)) {
				wleputils::ExceptionUtil::printErrorMsg("ExitProcess failed", GetLastError());
				return false;
			}

			// Wait until child process exits.
			if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED) {
				wleputils::ExceptionUtil::printErrorMsg("Wait for exit process failed", GetLastError());
				return false;
			}

			// Close process
			if (!CloseHandle(pi.hProcess)) {
				wleputils::ExceptionUtil::printErrorMsg("Cannot close process handle", GetLastError());
				return false;
			} else {
				pi.hProcess = NULL;
			}

			// Close thread handles
			if (!CloseHandle(pi.hThread)) {
				wleputils::ExceptionUtil::printErrorMsg("Cannot close thread handle", GetLastError());
				return false;
			} else {
				pi.hProcess = NULL;
			}
#ifdef DEBUG
			std::cout << "[OK] Successfully stopped a child process" << std::endl;
#endif // DEBUG
			return true;
		}
	};
}

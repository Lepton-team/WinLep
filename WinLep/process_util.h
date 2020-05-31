#pragma once
#ifdef _WIN32 // Only for windows 

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <string>

namespace wlep {

	class ProcessUtil {
	public:
		static PROCESS_INFORMATION launchProcess(const std::string &executable, const std::string &args) {
			STARTUPINFO start_info;
			PROCESS_INFORMATION proc_info; // Return value

			ZeroMemory(&start_info, sizeof(start_info));
			start_info.cb = sizeof(start_info);
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
			if (!CreateProcessW(
				app_exe,        // app path
				args_concat,    // Command line (needs to include app path as first argument. args seperated by whitepace)
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory
				(LPSTARTUPINFOW)&start_info,  // Pointer to STARTUPINFO structure
				&proc_info)     // Pointer to PROCESS_INFORMATION structure
				) {
				std::cout << "[ERROR] CreateProcess failed with error " << GetLastError() << "\n";
				throw std::exception("Could not create child process");
			} else {
				std::cout << "[SUCCESS] Successfully launched a child process" << std::endl;
			}

			return proc_info;
		}

		static bool isProcessActive(PROCESS_INFORMATION pi) {
			// Check if handle is closed
			if (pi.hProcess == NULL) {
				std::cout << "[ERROR] Process handle is closed or invalid (" << GetLastError() << ")\n";
				return false;
			}

			// If handle open, check if process is active
			DWORD lpExitCode = 0;
			if (GetExitCodeProcess(pi.hProcess, &lpExitCode) == 0) {
				std::cout << "[ERROR] Cannot return exit code (" << GetLastError() << ")\n";
				throw std::exception("Cannot return exit code");
			} else {
				return lpExitCode == STILL_ACTIVE;
			}
		}

		static bool stopProcess(PROCESS_INFORMATION &pi) {
			// Check if handle is invalid or has allready been closed
			if (pi.hProcess == NULL) {
				std::cout << "[ERROR] Process handle invalid. It was probably already closed (" << GetLastError() << ")\n";
				return false;
			}

			// Terminate Process
			if (!TerminateProcess(pi.hProcess, 1)) {
				std::cout << "[ERROR] ExitProcess failed (" << GetLastError() << ")\n";
				return false;
			}

			// Wait until child process exits.
			if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED) {
				std::cout << "[ERROR] Wait for exit process failed (" << GetLastError() << ")\n";
				return false;
			}

			// Close process
			if (!CloseHandle(pi.hProcess)) {
				std::cout << "[ERROR] Cannot close process handle (" << GetLastError() << ")\n";
				return false;
			} else {
				pi.hProcess = NULL;
			}

			// Close thread handles
			if (!CloseHandle(pi.hThread)) {
				std::cout << "[ERROR] Cannot close thread handle (" << GetLastError() << ")\n";
				return false;
			} else {
				pi.hProcess = NULL;
			}

			std::cout << "[SUCCESS] Successfully stopped a child process" << std::endl;
			return true;
		}
	};
}
#endif //win32


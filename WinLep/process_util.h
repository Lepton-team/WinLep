#pragma once

#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <string>
#include <vector>
#include "exception_util.h"
#include "string_util.h"
#include "vector_util.h"

namespace wleputils {

	class ProcessUtil {
	public:
		static void createPipes(HANDLE &child_in_read, HANDLE &child_in_write,
								HANDLE &child_out_read, HANDLE &child_out_write, SECURITY_ATTRIBUTES &sec_attr) {
			if (!CreatePipe(&child_out_read, &child_out_write, &sec_attr, 0)) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while creating 'out' pipe");
			}

			// Ensure the read handle to the pipe for STDOUT is not inherited.
			if (!SetHandleInformation(child_out_read, HANDLE_FLAG_INHERIT, 0)) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("The 'out' pipe is already inherited!");
			}

			if (!CreatePipe(&child_in_read, &child_in_write, &sec_attr, 0)) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while creating 'in' pipe");
			}

			// Ensure the write handle to the pipe for STDIN is not inherited. 
			if (!SetHandleInformation(child_in_write, HANDLE_FLAG_INHERIT, 0)) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("The 'in' pipe is already inherited!");
			}
		}

		static void closeProcessAndThreadHandle(PROCESS_INFORMATION &proc_info) {
			if (!CloseHandle(proc_info.hProcess)) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while closing process handle!", GetLastError());
			}

			if (!CloseHandle(proc_info.hThread)) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while closing thread handle!", GetLastError());
			}
		}

		static bool launchProcess(const std::string &executable, const std::string &args,
								  HANDLE &child_out_write, HANDLE &child_in_read) {
			PROCESS_INFORMATION proc_info;
			STARTUPINFOW start_info;

			ZeroMemory(&proc_info, sizeof(proc_info));

			ZeroMemory(&start_info, sizeof(start_info));
			start_info.cb = sizeof(start_info);
			start_info.hStdError = stderr; // GetStdHandle(STD_ERROR_HANDLE); // TODO: Suprress ?
			start_info.hStdOutput = child_out_write;
			start_info.hStdInput = child_in_read; // TODO: input_file_handle ?
			start_info.dwFlags |= STARTF_USESTDHANDLES;

			// Prepare CreateProcess args
			std::wstring exe = wleputils::StringUtil::toWideString(executable);
			std::wstring w_args = wleputils::StringUtil::toWideString(args);
			std::wstring input = exe + L" " + w_args;
			wchar_t *args_concat = const_cast<wchar_t *>(input.c_str());

			// Start the child process.
			// With CreateProcessW there's no need to set the arv[0] as the executable (itself)
			if (!CreateProcessW(
				NULL,           // app path
				args_concat,    // Command line (needs to include app path as first argument. args seperated by whitepace)
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				TRUE,           // Set handle inheritance to TRUE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory
				&start_info,    // Pointer to STARTUPINFO structure
				&proc_info)     // Pointer to PROCESS_INFORMATION structure
				) {
				return false;
			} else {
				closeProcessAndThreadHandle(proc_info);
				return true;
			}
		}

		/*
			Write a vector of bytes to pipe		
		*/
		static void writeToPipe(std::vector<BYTE> &data, HANDLE &child_write) {
			DWORD written_bytes = 0;
			if (!WriteFile(child_write, data.data(), data.size(), &written_bytes, NULL)) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while writing data to pipe!", GetLastError());
			}
			
			// Close the pipe handle so the child process stops reading. 
			if (!CloseHandle(child_write)) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while closing child handle!", GetLastError());
			}
		}

		/*
			Write a given file to pipe
		*/
		static void writeToPipe(HANDLE &input_file, HANDLE &child_write) {
			constexpr unsigned int buff_size = 1024 * 4; // 4 KB - size of one page
			DWORD read_bytes = 0;
			DWORD written_bytes = 0;
			BYTE *buff = new BYTE[buff_size];
			bool success = false;

			while (true) {
				success = ReadFile(input_file, buff, buff_size, &read_bytes, NULL);
				if (!success || read_bytes == 0) {
					break;
				}

				success = WriteFile(child_write, buff, read_bytes, &written_bytes, NULL);
				if (!success) {
					break;
				}
			}

			delete[] buff;

			// Close the pipe handle so the child process stops reading. 
			if (!CloseHandle(child_write)) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while closing child handle!", GetLastError());
			}

			if (!CloseHandle(input_file)) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while closing file handle!", GetLastError());
			}
		}

		/*
			Read output from the child process's pipe for STDOUT
			and write to the parent process's pipe for STDOUT.
			Stop when there is no more data.
		*/
		static void readFromPipeToBuffer(HANDLE &child_read, std::vector<BYTE> &data) {
			constexpr unsigned int buff_size = 1024 * 4; // 4 KB - size of one page
			DWORD read_bytes = 0;
			DWORD written_bytes = 0;
			BYTE *buff = new BYTE[buff_size];
			bool success = false;

			while (true) {
				success = ReadFile(child_read, buff, buff_size, &read_bytes, NULL);
				if (!success || read_bytes == 0) {
					break;
				}
				// Write the buffer to data
				data.insert(data.end(), &buff[0], &buff[read_bytes]);
			}

			delete[] buff;
		}
	};
}

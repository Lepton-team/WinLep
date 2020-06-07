//#define DEBUG

#include "directory.h"
#include "exception_util.h"
#include "file_util.h"
#include "string_util.h"
#include <Windows.h>
#include <algorithm>
#include <initializer_list>

void findFiles(const std::wstring &directory, std::vector<std::wstring> &files, bool recursive) {
	std::wstring tmp = directory + L"\\*";
	WIN32_FIND_DATAW file;
	HANDLE search_handle = FindFirstFileW(tmp.c_str(), &file);
	if (search_handle != INVALID_HANDLE_VALUE) {
		std::vector<std::wstring> directories;
		do {
			if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// Ignore . and .. directories
				if (!lstrcmpW(file.cFileName, L".") || !lstrcmpW(file.cFileName, L"..")) {
					continue;
				}
			} else {
				tmp = directory + L"\\" + std::wstring(file.cFileName);
				files.push_back(tmp);
			}
			tmp = directory + L"\\" + std::wstring(file.cFileName);
			if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				directories.push_back(tmp);
			}
		} while (FindNextFileW(search_handle, &file));

		if (!FindClose(search_handle)) {
			wleputils::ExceptionUtil::throwAndPrintException
				<std::exception>("Error while closing handle used for finding files!");
		}
		if (recursive) {
			for (auto it = directories.begin(), end = directories.end(); it != end; it++) {
				findFiles(*it, files, recursive);
			}
		}
	}
}

wlep::Directory::Directory(const std::string &dir_path, bool recursive, bool ommit_current_directory) {
	if (dir_path.empty()) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::invalid_argument>("Directory path cannot be empty!");
	}
	// Check that the input path is not longer than MAX_PATH.
	if (dir_path.length() > MAX_PATH) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::invalid_argument>("Directory path is too long!");
	}
	// Always ommit the current directory from the filename path
	if (dir_path == ".") {
		ommit_current_directory = true;
	}

	this->dir_path_ = wleputils::StringUtil::toWideString(dir_path);
	this->files_ = std::vector<std::string>();

	std::vector<std::wstring> wFiles;

	findFiles(this->dir_path_, wFiles, recursive);
	// Well ...
	for (const auto &file : wFiles) {
		std::string str_file = wleputils::StringUtil::wideStringToString(file);
		if (ommit_current_directory) {
			auto idx = str_file.find(dir_path);
			if (idx != std::string::npos) {
				// + 1 for the '\'
				str_file = str_file.substr(idx + dir_path.length() + 1);
			}
		}
		this->files_.push_back(str_file);
	}
}

std::vector<std::string> wlep::Directory::getAllFiles(std::initializer_list<std::string> file_extensions) {
	std::vector<std::string> result(this->files_.size());

	// Copy all the files that end with the given file extensions
	auto it = std::copy_if(this->files_.begin(), this->files_.end(), result.begin(),
						   [file_extensions](std::string &file) {
							   bool is_matched = false;
							   for (auto extension : file_extensions) {
								   auto ex = wleputils::FileUtil::getFileExtension(file);
								   wleputils::StringUtil::toLowerCase(ex);
								   is_matched |= ex == extension;
							   }
							   return is_matched;
						   });
	// Shrink to new size
	result.resize(std::distance(result.begin(), it));

	return result;
}

std::vector<std::string> wlep::Directory::getAllFiles() {
	return this->files_;
}

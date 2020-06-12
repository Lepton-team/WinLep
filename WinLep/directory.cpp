//#define DEBUG

#include "directory.h"
#include "exception_util.h"
#include "file_util.h"
#include "string_util.h"
#include <Windows.h>
#include <algorithm>
#include <initializer_list>

wlep::Directory::Directory(const std::string &dir_path, std::vector<std::string> &file_extensions,
						   bool recursive, bool omit_current_directory) {
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
		omit_current_directory = true;
	}

	this->dir_path_ = wleputils::StringUtil::toWideString(dir_path);
	this->files_ = std::vector<std::string>();

	std::vector<std::wstring> wFiles;
	std::vector<std::wstring> wExtensions;

	for (const auto &str : file_extensions) {
		wExtensions.push_back(wleputils::StringUtil::toWideString(str));
	}

	wleputils::FileUtil::findFiles(this->dir_path_, wFiles, wExtensions, recursive);
	// Well ...
	for (const auto &file : wFiles) {
		std::string str_file = wleputils::StringUtil::wideStringToString(file);
		if (omit_current_directory) {
			auto idx = str_file.find(dir_path);
			if (idx != std::string::npos) {
				// + 1 for the '\'
				str_file = str_file.substr(idx + dir_path.length() + 1);
			}
		}
		this->files_.push_back(str_file);
	}
}

wlep::Directory::Directory(const std::string &dir_path, bool recursive, std::vector<std::string> &file_extensions, bool omit_current_directory) {
	Directory(dir_path, file_extensions, recursive, omit_current_directory);
}

std::vector<std::string> wlep::Directory::getAllFiles(std::vector<std::string> &file_extensions) {
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

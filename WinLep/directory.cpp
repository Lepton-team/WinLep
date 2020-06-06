//#define DEBUG

#include "directory.h"
#include "exception_util.h"
#include "file_util.h"
#include "string_util.h"
#include <Windows.h>
#include <strsafe.h>
#include <algorithm>
#include <initializer_list>

wlep::Directory::Directory(const std::string &dir_path) {
	if (dir_path.empty()) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::invalid_argument>("Directory path cannot be empty!");
	}
	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.
	if (dir_path.length() > (MAX_PATH - 3)) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::invalid_argument>("Directory path is too long!");
	}
	this->dir_path_ = dir_path;
#ifdef DEBUG
	std::cerr << "Target directory: " << dir_path << "\n";
#endif // DEBUG

	char *dir = new char[MAX_PATH];

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.
	StringCchCopyA(dir, MAX_PATH, dir_path.c_str());
	StringCchCatA(dir, MAX_PATH, "\\*");

	WIN32_FIND_DATA find_data;
	HANDLE find_handle = FindFirstFileA(dir, &find_data);

	if (find_handle == INVALID_HANDLE_VALUE) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::exception>("Error while finding first file!");
	}
#ifdef DEBUG
	LARGE_INTEGER filesize;
#endif // DEBUG

	this->files_ = std::vector<std::string>();
	this->subdirectory_names_ = std::vector<std::string>();

	// List all files and subdirectories in this directory 
	do {
		if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
#ifdef DEBUG
			std::cerr << "<DIR> " << find_data.cFileName << "\n";
#endif // DEBUG
			this->subdirectory_names_.push_back(find_data.cFileName);
			// It's a file
		} else {
			std::string filename = find_data.cFileName;
			if (filename[0] != '.') {
				this->files_.push_back(find_data.cFileName);
			}
#ifdef DEBUG
			filesize.LowPart = find_data.nFileSizeLow;
			filesize.HighPart = find_data.nFileSizeHigh;
			std::cerr << find_data.cFileName << "\t" << filesize.QuadPart << " B\n";
#endif // DEBUG

		}
	} while (FindNextFileA(find_handle, &find_data));

	delete[] dir;

	if (GetLastError() != ERROR_NO_MORE_FILES) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::exception>("Error while getting files from directory!");
	}

	if (!FindClose(find_handle)) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::exception>("Error while closing handle used for finding files!");
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

std::vector<std::string> wlep::Directory::getAllSubDirectoryNames() {
	return this->subdirectory_names_;
}

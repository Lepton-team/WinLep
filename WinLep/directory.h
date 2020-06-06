#pragma once
#include <string>
#include <vector>

namespace wlep {

	class Directory {
	private:
		std::string dir_path_;
		std::vector<std::string> files_;
		std::vector<std::string> subdirectory_names_;
		std::vector<wlep::Directory> subdirectories_; // TODO ?

	public:
		Directory(const std::string &dir_path);
		std::vector<std::string> getAllFiles(const std::string &file_extension);
		std::vector<std::string> getAllFiles();
		std::vector<std::string> getAllSubDirectoryNames();

	};
}


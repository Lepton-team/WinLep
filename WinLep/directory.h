#pragma once
#include <string>
#include <vector>

namespace wlep {

	class Directory {
	private:
		std::wstring dir_path_;
		std::vector<std::string> files_;
	public:
		Directory(const std::string &dir_path, bool recursive = false, bool ommit_current_directory = false);
		std::vector<std::string> getAllFiles(std::vector<std::string> &file_extensions);
		std::vector<std::string> getAllFiles();
	};
}


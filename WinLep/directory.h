#pragma once
#include <string>
#include <vector>

namespace wlep {

	class Directory {
	private:
		std::wstring dir_path_;
		std::vector<std::string> files_;

	public:
		Directory(const std::string &dir_path, bool recursive = false, bool ommit_current_directory = true);
		std::vector<std::string> getAllFiles(const std::initializer_list<std::string>file_extension);
		std::vector<std::string> getAllFiles();
	};
}


#pragma once
#include <string>
#include <vector>

namespace wlep {

	class Directory {
	private:
		static std::vector<std::string> EMPTY_VECTOR_;
		std::wstring dir_path_;
		std::vector<std::string> files_;
	public:
		Directory(const std::string &dir_path, std::vector<std::string> &file_extensions = EMPTY_VECTOR_, bool recursive = false, bool omit_current_directory = false);
		Directory(const std::string &dir_path, bool recursive = false, std::vector<std::string> &file_extensions = EMPTY_VECTOR_, bool omit_current_directory = false);


		std::vector<std::string> getAllFiles(std::vector<std::string> &file_extensions);
		std::vector<std::string> getAllFiles();
	};
}


#pragma once
#include <string>
#include <map>
typedef std::basic_fstream<unsigned char, std::char_traits<unsigned char>> uFstream;
typedef unsigned char uChar;

namespace wlepconstants {
	//
	static constexpr unsigned int version_size = 3; // Bytes
	static constexpr unsigned int thumbnail_size_size = 4; // Bytes
	static constexpr unsigned int exif_size_size = 4; // Bytes
	static constexpr unsigned int header_prefix_size = 2; // Bytes

	static constexpr unsigned char header_prefix[header_prefix_size] = {0xC6, 0xD6};

	static constexpr unsigned int thumbnail_side_length = 256;

	static const std::string file_extension = ".wlep";
	static const std::string jpg_extension = ".jpg";
	static const std::wstring jpg_format = L"image/jpeg";

	static const std::string sup_extensions = ".jpg, .jpeg, .jpe, .jif, .jfif, .jfi, " + file_extension;

	static const std::map<std::string, std::wstring> sup_file_extension_img_format_map = {
		{"jpeg", jpg_format},
		{"jpg", jpg_format},
		{"jpe", jpg_format},
		{"jfif", jpg_format},
		{"jif", jpg_format},
		{"jfi", jpg_format}
	};

	// [0] -> Main version number
	// [1] -> Sub-version number
	// [2] -> Revision number
	static constexpr unsigned char version[version_size] = {1, 1, 2};
}

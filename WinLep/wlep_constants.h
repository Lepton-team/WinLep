#pragma once
#include <string>

typedef std::basic_fstream<unsigned char, std::char_traits<unsigned char>> uFstream;
typedef unsigned char uChar;

namespace wlepconstants {
	//
	static constexpr unsigned int version_size = 3; // Bytes
	static constexpr unsigned int thumbnail_size_size = 4; // Bytes
	static constexpr unsigned int exif_size_size = 4; // Bytes
	static constexpr unsigned int header_prefix_size = 2; // Bytes

	static constexpr unsigned char header_prefix[header_prefix_size] = {0xC6, 0xD6};

	static const std::string file_extension = ".wlep";

	// [0] -> Main version number
	// [1] -> Sub-version number
	// [2] -> Revision number
	static constexpr unsigned char version[version_size] = {1, 1, 2};
}

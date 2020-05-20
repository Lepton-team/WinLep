#pragma once
#include <string>

typedef std::basic_fstream<unsigned char, std::char_traits<unsigned char>> uFstream;
typedef unsigned char uChar;

namespace WLepConstants {
	static const unsigned int version_size = 3; // Bytes
	static const unsigned int thumbnail_size_size = 4; // Bytes
	static const unsigned int exif_size_size = 4; // Bytes
	static const unsigned int header_prefix_size = 2; // Bytes

	static const unsigned char header_prefix[header_prefix_size] = {0xC6, 0xD6};

	static const std::string file_extension = ".wlep";

	// [0] -> Main version number
	// [1] -> Sub-version number
	// [2] -> Revision number
	static const unsigned char version[version_size] = {1, 1, 2};
}

#pragma once

namespace WLepConstants {
	static const unsigned int version_size = 3; // Bytes
	static const unsigned int thumbnail_size_size = 4; // Bytes
	static const unsigned int exif_size_size = 4; // Bytes
	static const unsigned int header_prefix_size = 2; // Bytes

	static const unsigned char header_prefix[header_prefix_size] = {0xC6, 0xD6};
	// [0] -> Main version number
	// [1] -> Sub-version number
	// [2] -> Revision number
	static const unsigned char version[version_size] = {1, 1, 2};
}

#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "wlep_constants.h"

namespace wlep {
	class WLepHeader {
	private:
		std::streampos calculateFileSize(std::ifstream &file);
		std::vector<unsigned char> createThumbnailData(std::ifstream &thumbnail);
		bool isBigEndian();

	public:
		std::vector<unsigned char> thumbnail_size_arr;
		std::vector<unsigned char> exif_size_arr;

		std::vector<unsigned char> thumbnail_data;
		std::vector<unsigned char> exif_data;
		std::vector<unsigned char> data;
		std::vector<unsigned char> header_prefix;
		std::vector<unsigned char> version;

		size_t thumbnail_size = 0;
		size_t exif_size = 0;

		WLepHeader(std::string const &thumbnail_filename);
		WLepHeader();
	};
};
#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "wlep_constants.h"
#include <Windows.h>

namespace wlep {
	class WLepHeader {
	private:
		void createThumbnailData(IStream *thumbnail_data_stream, ULONGLONG size);
		void createThumbnailSize(size_t size);

		bool isBigEndian();

	public:
		std::vector<unsigned char> thumbnail_size_arr;
		std::vector<unsigned char> exif_size_arr;

		std::vector<unsigned char> thumbnail_data;
		std::vector<unsigned char> exif_data;
		std::vector<unsigned char> header_prefix;
		std::vector<unsigned char> version;

		size_t thumbnail_size = 0;
		size_t exif_size = 0;

		WLepHeader(IStream *thumbnail_data_stream);
		WLepHeader();
	};
};
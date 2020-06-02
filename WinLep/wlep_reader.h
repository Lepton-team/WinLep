#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "wlep_header.h"

namespace wlep {
	class WLepReader {
	private:
		std::string filename_;
		uFstream file_;

		const std::streampos prefix_start_pos_ = 0;
		const std::streampos version_start_pos_ = prefix_start_pos_ + std::streampos(wlepconstants::header_prefix_size);
		const std::streampos thumbnail_size_start_pos_ = version_start_pos_ + std::streampos(wlepconstants::version_size);
		const std::streampos thumbnail_start_pos_ = thumbnail_size_start_pos_ + std::streampos(wlepconstants::thumbnail_size_size);

		void readFromFileToVector(std::vector<unsigned char> &vec, size_t bytes);

	public:
		wlep::WLepHeader header;

		WLepReader(const std::string &filename);
		~WLepReader();

		std::string debug_str();
		void readHeader();
	};
}

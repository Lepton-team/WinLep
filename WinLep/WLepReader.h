#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "WLepHeader.h"
#include "VectorUtil.h"

namespace WLep {
	class WLepReader {
	private:
		const std::string const filename;
		uFstream file;

		const std::streampos prefix_start_pos = 0;
		const std::streampos version_start_pos = prefix_start_pos + std::streampos(WLepConstants::header_prefix_size);
		const std::streampos thumbnail_size_start_pos = version_start_pos + std::streampos(WLepConstants::version_size);
		const std::streampos thumbnail_start_pos = thumbnail_size_start_pos + std::streampos(WLepConstants::thumbnail_size_size);

		void read_from_file_to_vec(std::vector<unsigned char> &vec, size_t bytes);
		void close_fstream();

	public:
		WLep::WLepHeader header;

		WLepReader(const std::string &filename);
		~WLepReader();

		std::string debug_str();
		void read_header();
	};
}

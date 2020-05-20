#pragma once

#include <fstream>
#include "WLepConstants.h"
#include "WLepHeader.h"
#include "FileUtils.h"
#include "StringUtil.h"

namespace WLep {
	class WLepWriter {
	private:
		uFstream file;
		std::string filename;

		void open_fstream(std::string &filename);
		void close_fstream();

	public:
		WLep::WLepHeader header;

		WLepWriter(std::string &filename, WLep::WLepHeader &header);
		WLepWriter(std::string &filename, const std::string &thumbnail_filename);

		~WLepWriter();

		size_t write_header();
		std::string debug_str();
	};
}

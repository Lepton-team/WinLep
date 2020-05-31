#pragma once

#include <fstream>
#include "WLepConstants.h"
#include "WLepHeader.h"
#include "FileUtils.h"
#include "StringUtil.h"

namespace wlep {
	class WLepWriter {
	private:
		uFstream file_;
		std::string filename_;

		void openFileStream(std::string &filename);
		void closeFileStream();

	public:
		wlep::WLepHeader header;

		WLepWriter(std::string &filename, wlep::WLepHeader &header);
		WLepWriter(std::string &filename, const std::string &thumbnail_filename);

		~WLepWriter();

		size_t writeHeader();
		std::string debug_str();
	};
}

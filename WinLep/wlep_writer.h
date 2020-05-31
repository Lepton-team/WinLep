#pragma once

#include <fstream>
#include "wlep_constants.h"
#include "wlep_header.h"
#include "file_util.h"
#include "string_util.h"

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

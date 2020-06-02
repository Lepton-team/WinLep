#pragma once

#include <fstream>
#include "wlep_header.h"

namespace wlep {
	class WLepWriter {
	private:
		uFstream file_;
		std::string filename_;

	public:
		wlep::WLepHeader header;

		WLepWriter(std::string &filename, wlep::WLepHeader &header);
		WLepWriter(std::string &filename, const std::string &thumbnail_filename);

		~WLepWriter();

		size_t writeHeader();
		std::string debug_str();
	};
}

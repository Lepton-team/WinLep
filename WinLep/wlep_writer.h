#pragma once

#include <fstream>
#include "wlep_header.h"
#include <Windows.h>

namespace wlep {
	class WLepWriter {
	private:
		uFstream file_;
		std::string filename_;

	public:
		wlep::WLepHeader header;

		WLepWriter(std::string &filename, wlep::WLepHeader &header);
		WLepWriter(std::string &filename, IStream *thumbnail_data);

		~WLepWriter();

		size_t writeHeader();
		std::string debug_str();
	};
}

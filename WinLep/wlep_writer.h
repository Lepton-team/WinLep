#pragma once

#include <fstream>
#include "wlep_header.h"

namespace wlep {
	class WLepWriter {
	private:
		uFstream file_;
		std::string out_filename_;
		std::string jpg_filename_;

		size_t writeHeader();
		size_t writeLeptonData();

	public:
		wlep::WLepHeader header;
		WLepWriter(std::string &out_filename, wlep::WLepHeader &header);
		WLepWriter(std::string &out_filename, IStream *thumbnail_data);
		WLepWriter(std::string &out_filename, std::string &jpg_filename);

		~WLepWriter();

		size_t writeWinLepFile();
		std::string debug_str();
	};
}

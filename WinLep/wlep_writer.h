#pragma once

#include <fstream>
#include "wlep_header.h"

namespace wlep {
	class WLepWriter {
	private:
		uFstream wlep_file_;
		std::string wlep_filename_;
		std::string jpg_filename_;

		size_t writeHeader();
		size_t writeLeptonData();

	public:
		wlep::WLepHeader header;
		WLepWriter(std::string &wlep_filename, wlep::WLepHeader &header);
		WLepWriter(std::string &wlep_filename, IStream *thumbnail_data);
		WLepWriter(std::string &wlep_filename, std::string &jpg_filename);

		~WLepWriter();

		size_t writeWinLepFile();
		std::string debug_str();
	};
}

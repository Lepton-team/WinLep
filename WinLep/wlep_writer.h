#pragma once

#include <fstream>
#include "wlep_header.h"

namespace wlep {
	class WLepWriter {
	private:
		uFstream wlep_file_;
		uFstream jpg_file_;
		std::string wlep_filename_;
		std::string jpg_filename_;

		size_t writeHeader();
		size_t writeLeptonData();

	public:
		wlep::WLepHeader header;
		WLepWriter(const std::string &wlep_filename, const std::string &jpg_filename, bool create_thumbnail = true);
		~WLepWriter();

		size_t writeWinLepFile();
		size_t writeJpgFile(std::vector<uChar> &lepton_data);
		std::string debug_str();
	};
}

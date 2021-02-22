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
		std::wstring module_dir_path_;
		std::wstring lepton_exe_;
		std::wstring lepton_args_;

		size_t writeHeader();
		size_t writeLeptonData();

	public:
		wlep::WLepHeader header;
		WLepWriter(const std::string &wlep_filename, const std::string &jpg_filename, 
				   const std::wstring &module_dir_path, const std::wstring &lepton_exe,
				   const std::wstring &lepton_args, bool create_thumbnail = true);
		~WLepWriter();

		size_t writeWinLepFile();
		size_t writeJpgFile(std::vector<uChar> &lepton_data, bool clear_lepton_data = true);
		std::string debug_str();
	};
}

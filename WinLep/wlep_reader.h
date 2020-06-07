#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "wlep_header.h"

namespace wlep {
	class WLepReader {
	private:
		std::string filename_;
		uFstream file_;

		void readFromFileToVector(std::vector<unsigned char> &vec, size_t bytes);

	public:
		wlep::WLepHeader header;

		WLepReader(const std::string &filename);
		~WLepReader();

		std::string debug_str();
		void readAndValidateHeader(bool read_thumbnail = true);
		std::vector<unsigned char> validateFileAndReadLeptonData();
	};
}

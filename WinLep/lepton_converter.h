#pragma once
#include <vector>
#include <string>

namespace wlep {
	class LeptonConverter {
	public:
		std::vector<unsigned char> convertJpgToLepton(const std::string &jpg_filename);
		std::vector<unsigned char> convertLeptonToJpg(std::vector<unsigned char> &lepton_data);
	};
}
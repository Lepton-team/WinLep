#pragma once
#include <vector>
#include <string>

namespace wlep {
	class LeptonConverter {
	private:
		std::wstring lepton_exe_;
		std::wstring lepton_args_;

	public:
		LeptonConverter(const std::wstring &lepton_exe, const std::wstring &lepton_args);
		std::vector<unsigned char> convertJpgToLepton(const std::wstring &jpg_filename);
		std::vector<unsigned char> convertLeptonToJpg(std::vector<unsigned char> &lepton_data);
	};
}

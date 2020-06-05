#pragma once
#include <vector>
#include <string>

namespace wlep {
	class LeptonConvertor {
	public:
		std::vector<unsigned char> convertJpgToLepton(const std::string &jpg_filename);
	};
}
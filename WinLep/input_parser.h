#pragma once
#include <vector>
#include <string>

namespace wlep {
	class InputParser {
	private:
		std::vector<std::string> tokens;

	public:
		InputParser(int &argc, char **argv);
		const std::string &getFirstCmdOption(const std::string &option) const;
		const std::string &getSecondCmdOption(const std::string &option) const;

		bool cmdOptionExists(const std::string &option) const;
	};
}


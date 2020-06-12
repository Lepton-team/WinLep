#pragma once
#include <vector>
#include <string>

namespace wlep {
	class InputParser {
	private:
		// Flags e.g. -f --help
		std::vector<std::string> flags_;
		// Other stuff, e.g. filenames, directories etc.
		std::vector<std::string> options_;

	public:
		InputParser(int &argc, char **argv);

		std::string getOption(const int &index);
		bool cmdFlagExists(const std::string &option) const;
	};
}


#include "input_parser.h"

wlep::InputParser::InputParser(int &argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		this->tokens.push_back(std::string(argv[i]));
	}
}

 std::string wlep::InputParser::getFirstCmdOption(const std::string &option) {
	auto it = std::find(this->tokens.begin(), this->tokens.end(), option);
	if (it != this->tokens.end() && ++it != this->tokens.end()) {
		return *it;
	}
	return "";
}

std::string wlep::InputParser::getSecondCmdOption(const std::string &option) {
	auto it = std::find(this->tokens.begin(), this->tokens.end(), option);

	if (it != this->tokens.end() && (it += 2) != this->tokens.end()) {
		return *it;
	}
	return "";
}

bool wlep::InputParser::cmdOptionExists(const std::string &option) const {
	return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
}

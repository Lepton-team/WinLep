#include "input_parser.h"

wlep::InputParser::InputParser(int &argc, char **argv) {
	/*
	  Supports mutiple flags in one arg (POSIX style) e.g. -jdr
	  Always ommits the first '-'
	  Parsed flags may look like following: {'v', 'd', '-help', '-verbose', 'f'}
	*/
	for (int i = 1; i < argc; i++) {
		std::string str_argv(argv[i]);
		// It's a flag
		if (str_argv[0] == '-') {
			if (str_argv.size() < 2) {
				continue;
			}
			// Ommit the first '-'
			str_argv = str_argv.substr(1);
			// If argv starts with --
			if (str_argv[0] == '-') {
				this->tokens_.push_back(str_argv);
			} else {
				for (int i = 0; i < str_argv.size(); i++) {
					this->tokens_.push_back(std::string(1, str_argv[i]));
				}
			}
			// It's a me Mario
		} else {
			this->options_.push_back(str_argv);
		}
	}
}

std::string wlep::InputParser::getOption(const int &index) {
	if (index > this->options_.size() - 1) {
		return "";
	}
	return this->options_[index];
}

bool wlep::InputParser::cmdOptionExists(const std::string &option) const {
	return std::find(this->tokens_.begin(), this->tokens_.end(), option) != this->tokens_.end();
}

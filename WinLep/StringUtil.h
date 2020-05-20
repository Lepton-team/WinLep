#pragma once
#include <string>

namespace WLepUtils {
	static class StringUtil {
	public:
		/*
			Finds out whether a string ends with a given suffix
		*/
		static inline bool ends_with(const std::string &str, const std::string &suffix) {
			return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
		}
	};
}
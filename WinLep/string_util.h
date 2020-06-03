#pragma once
#include <string>
#include <algorithm>
namespace wleputils {
	static class StringUtil {
	public:
		/*
			Finds out whether a string ends with a given suffix
		*/
		static inline bool endsWith(const std::string &str, const std::string &suffix) {
			return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
		}

		static inline void toLowerCase(std::string &str) {
			std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
				return std::tolower(c);
						   });
		}

	};
}
#pragma once
#include <string>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <vector>

namespace wleputils {
	static class StringUtil {
	public:

		static inline bool endsWith(const std::string &str, const std::string &suffix) {
			return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
		}

		static inline void toLowerCase(std::string &str) {
			std::transform(str.begin(), str.end(), str.begin(),
						   [](unsigned char c) {
							   return std::tolower(c);
						   });
		}
		// convert UTF-8 string to wstring
		static inline std::wstring toWideString(const std::string &str) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
			return convertor.from_bytes(str);
		}

		// convert wstring to UTF-8 string
		static inline std::string wideStringToString(const std::wstring &str) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
			return convertor.to_bytes(str);
		}

		static std::vector<std::string> split(const std::string &str, const std::string &delim) {
			std::vector<std::string> tokens;
			size_t prev = 0, pos = 0;
			do {
				pos = str.find(delim, prev);
				if (pos == std::string::npos) pos = str.length();
				std::string token = str.substr(prev, pos - prev);
				if (!token.empty()) tokens.push_back(token);
				prev = pos + delim.length();
			} while (pos < str.length() && prev < str.length());

			return tokens;
		}

	};
}
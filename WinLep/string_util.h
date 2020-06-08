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
		
		/*
			Convert UTF-8 string to wstring
		*/
		static inline std::wstring toWideString(const std::string &str) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
			return convertor.from_bytes(str);
		}

		/*
			Convert wstring to UTF-8 string
		*/
		static inline std::string wideStringToString(const std::wstring &str) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
			return convertor.to_bytes(str);
		}

		/*
			Splits a string to vector of substrings using the given delimiter
		*/
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

		/*
			Splits a wide string to vector of substrings using the given delimiter
		*/
		static std::vector<std::wstring> split(const std::wstring &str, const std::wstring &delim) {
			std::vector<std::wstring> tokens;
			size_t prev = 0, pos = 0;
			do {
				pos = str.find(delim, prev);
				if (pos == std::wstring::npos) pos = str.length();
				std::wstring token = str.substr(prev, pos - prev);
				if (!token.empty()) tokens.push_back(token);
				prev = pos + delim.length();
			} while (pos < str.length() && prev < str.length());

			return tokens;
		}

	};
}
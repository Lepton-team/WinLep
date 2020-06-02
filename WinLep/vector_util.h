#pragma once
#include <vector>
#include <string>
#include <sstream>

namespace wleputils {
	class VectorUtil {
	public:
		/*
			Appends to a vector
		*/
		template <typename T, typename V>
		static inline void append(T &t, std::vector<V> &vec) {
			vec.insert(vec.end(), std::begin(t), std::end(t));
		}

		/*
			Inserts and item to the beginning of a vector
		*/
		template <typename T, typename V>
		static inline void prepend(T &t, std::vector<V> &vec) {
			vec.insert(vec.begin(), std::begin(t), std::end(t));
		}

		/*
			Converts an array to a vector
		*/
		template <typename T>
		static inline void arrayToVector(std::vector<T> &vec, T *arr, const size_t size) {
			vec.assign(arr, arr + size);
		}

		/*
			Converts a vector of bytes to one hexadecimal string
		*/
		template <typename T>
		static inline std::string hexVectorToHexString(std::vector<T> &vec) {
			std::string hex_str = "";
			for (unsigned char c : vec) {
				std::stringstream stream;
				stream << std::hex << int(c);
				hex_str += stream.str();
			}

			return hex_str;
		}

		/*
			Converts a vector of bytes to an unsigned integer
		*/
		template <typename T>
		static inline size_t hexVectorToInt(std::vector<T> &vec) {
			std::string hex_str = wleputils::VectorUtil::hexVectorToHexString(vec);
			std::istringstream converter(hex_str);
			size_t value;
			converter >> std::hex >> value;

			return value;
		}
	};
}

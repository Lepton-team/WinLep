#pragma once
#include <vector>
#include <fstream>

typedef std::basic_fstream<unsigned char, std::char_traits<unsigned char>> uFstream;
typedef unsigned char uChar;

namespace VectorUtil {
	/*
		Appends to a vector
	*/
	template <typename T, typename V>
	inline void append(T &t, std::vector<V> &vec) {
		vec.insert(vec.end(), std::begin(t), std::end(t));
	}

	/*
		Inserts and item to the beginning of a vector
	*/
	template <typename T, typename V>
	inline void prepend(T &t, std::vector<V> &vec) {
		vec.insert(vec.begin(), std::begin(t), std::end(t));
	}

	/*
		Converts an array to vector
	*/
	template <typename T>
	inline void to_vector(std::vector<T> &vec, T *arr, const size_t size) {
		vec.assign(arr, arr + size);
	}
}

namespace FileUtil {
	/*
		Writes any vector to a file
	*/
	template<typename T>
	void write_to_file(uFstream &file, std::vector<T> &data) {
		file.write((uChar *)&data[0], sizeof(T) * data.size());
	}

	/*
		Writes any array to a file
	*/
	template<typename T>
	void write_to_file(uFstream &file, T *data, size_t size) {
		file.write((uChar *)&data[0], sizeof(T) * size);
	}

	/*
		Writes any single item to a file
	*/
	template<typename T>
	void write_to_file(uFstream &file, T &t) {
		file.write((uChar *)&t, sizeof(T));
	}
}

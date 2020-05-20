#pragma once
#include <vector>
#include <fstream>

typedef std::basic_fstream<unsigned char, std::char_traits<unsigned char>> uFstream;
typedef unsigned char uChar;

/*
	Once write fails, all further operations on the stream are no-ops, until the error is cleared.
	Including tellp, which is required to return -1 if any previous operation has failed.
*/
namespace WLepUtils {
	static class FileUtil {
	public:
		/*
			Writes any vector to a file
			Returns the bytes written
		*/
		template<typename T>
		static inline size_t write_to_file(uFstream &file, std::vector<T> &data) {
			size_t before = file.tellp();
			file.write((uChar *)&data[0], sizeof(T) * data.size());
			size_t pos = file.tellp();

			return pos < 0 ? -1 : pos - before;
		}

		/*
			Writes any array to a file
			Returns the bytes written
		*/
		template<typename T>
		static inline size_t write_to_file(uFstream &file, T *data, size_t size) {
			size_t before = file.tellp();
			file.write((uChar *)&data[0], sizeof(T) * size);
			size_t pos = file.tellp();

			return pos < 0 ? -1 : pos - before;
		}

		/*
			Writes any single item to a file
			Returns the bytes written
		*/
		template<typename T>
		static inline size_t write_to_file(uFstream &file, T &t) {
			size_t before = file.tellp();
			file.write((uChar *)&t, sizeof(T));
			size_t pos = file.tellp();

			return pos < 0 ? -1 : pos - before;
		}
	};
}

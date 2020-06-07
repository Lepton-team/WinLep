#pragma once
#include <vector>
#include <fstream>
#include "exception_util.h"
#include "string_util.h"
#include <Windows.h>

typedef std::basic_fstream<unsigned char, std::char_traits<unsigned char>> uFstream;
typedef unsigned char uChar;

namespace wleputils {
	class FileUtil {
	public:
		/*
			Closes a given file stream
		*/
		static inline void closeFileStream(uFstream &stream) {
			if (stream && !stream.is_open()) {
				stream.close();
			}
		}
		/*
			Opens a file via stream in binary mode
			If the path to the given filename doesn't exist and the file is open for writing
			it'll be created so the desired file can be created
		*/
		static void openFileStream(uFstream &stream, std::string &filename, int mode) {
			if (stream.is_open()) {
				return;
			}

			if (filename.empty()) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::invalid_argument>("Error while opening file stream!", "Filename cannot be empty!");
			}

			// Create file only when writing to it
			if (mode & std::ios::out) { 
				std::vector<std::string> split = wleputils::StringUtil::split(filename, "\\");
				std::string dir = "";
				// -1 Because the last one is the filename
				for (int i = 0; i < split.size() - 1; i++) {
					dir += split[i] + "\\";
					if (!directoryExists(dir)) {
						if (!CreateDirectoryA(dir.c_str(), NULL)) {
							std::string msg = "Error while creating folder " + dir;
							wleputils::ExceptionUtil::throwAndPrintException<std::exception>(msg);
						}
					}
				}
			}

			stream.open(filename, mode | std::ios::binary);

			if (!stream) {
				std::string msg = "Cannot open file " + filename;
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while opening file stream!", msg.c_str());
			}

			if (!stream.good()) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while opening file stream!", "Error while opening a file!");
			}

			// Stop eating new lines in binary mode!!!
			stream.unsetf(std::ios::skipws);
		}

		/*
			Returns a file extension of a given file
		*/
		static inline std::string getFileExtension(std::string &filename) {
			std::string::size_type idx;
			idx = filename.rfind('.');

			if (idx != std::string::npos) {
				return filename.substr(idx + 1);
			}

			return "";
		}

		/*
			Checks if a given directory exists
		*/
		static inline bool directoryExists(const std::string &path) {
			DWORD attr = GetFileAttributesA(path.c_str());
			return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
		}

		static inline std::string getFileNameWithoutExtension(const std::string &filename) {
			std::string::size_type idx;
			idx = filename.find('.');

			if (idx != std::string::npos) {
				return filename.substr(0, idx);
			}

			return "";
		}

		/*
			Once write fails, all further operations on the stream are no-ops, until the error is cleared.
			Including tellp, which is required to return -1 if any previous operation has failed.
		*/

		/*
			Writes any vector to a file
			Returns the bytes written
		*/
		template<typename T>
		static size_t writeToFile(uFstream &file, std::vector<T> &data) {
			size_t before = file.tellp();
			try {
				file.exceptions(std::fstream::failbit);
				file.write((uChar *)&data[0], sizeof(T) * data.size());
			} catch (std::ios_base::failure & ex) {
				std::string msg = "Error while writing to file!\n" + std::string(ex.what()) + "\n";
				wleputils::ExceptionUtil::printErrorMsg(msg);
			}
			size_t pos = file.tellp();

			return pos < 0 ? -1 : pos - before;
		}

		/*
			Writes any array to a file
			Returns the bytes written
		*/
		template<typename T>
		static size_t writeToFile(uFstream &file, T *data, size_t size) {
			size_t before = file.tellp();
			try {
				file.exceptions(std::fstream::failbit);
				file.write((uChar *)&data[0], sizeof(T) * size);
			} catch (std::ios_base::failure & ex) {
				std::string msg = "Error while writing to file!\n" + ex.what() + "\n";
				wleputils::ExceptionUtil::printErrorMsg(msg);
			}
			size_t pos = file.tellp();

			return pos < 0 ? -1 : pos - before;
		}

		/*
			Writes any single item to a file
			Returns the bytes written
		*/
		template<typename T>
		static size_t writeToFile(uFstream &file, T &t) {
			size_t before = file.tellp();
			try {
				file.exceptions(std::fstream::failbit);
				file.write((uChar *)&t, sizeof(T));
			} catch (std::ios_base::failure & ex) {
				std::string msg = "Error while writing to file!\n" + ex.what() + "\n";
				wleputils::ExceptionUtil::printErrorMsg(msg);
			}
			size_t pos = file.tellp();

			return pos < 0 ? -1 : pos - before;
		}
	};
}

#pragma once
#include <vector>
#include <fstream>
#include "exception_util.h"
#include "wlep_constants.h"
#include "string_util.h"
#include <Windows.h>

typedef std::basic_fstream<unsigned char, std::char_traits<unsigned char>> uFstream;
typedef unsigned char uChar;

namespace wleputils {
	class FileUtil {
	public:
		/**
			Close a given file stream
		*/
		static inline void closeFileStream(uFstream &stream) {
			if (stream && !stream.is_open()) {
				stream.close();
			}
		}
		/**
			Open a file via stream in binary mode
			If the path to the given filename doesn't exist and the file is open for writing
			it'll be created so the desired file can be created
		*/
		static void openFileStream(uFstream &stream, const std::string &filename, int mode) {
			if (stream.is_open()) {
				return;
			}

			if (filename.empty()) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::invalid_argument>("Error while opening file stream!", "Filename cannot be empty!");
			}

			// Create file only when writing to it
			if (mode & std::ios::out) {
				std::wstring wide_filename = wleputils::StringUtil::toWideString(filename);
				std::vector<std::wstring> split = wleputils::StringUtil::split(wide_filename, L"\\");
				std::wstring dir = L"";
				// -1 Because the last one is the filename
				for (int i = 0; i < split.size() - 1; i++) {
					dir += split[i] + L"\\";
					if (!directoryExistsW(dir)) {
						if (!CreateDirectoryW(dir.c_str(), NULL)) {
							std::string msg = "Error while creating folder " + wleputils::StringUtil::wideStringToString(dir);
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

		static inline bool isSupportedExtension(const std::string &extension) {
			return wlepconstants::sup_file_extension_img_format_map.find(extension)
				!= wlepconstants::sup_file_extension_img_format_map.end();
		}

		static std::vector<std::string> getSupportedExtensions() {
			std::vector<std::string> keys;
			for (auto it = wlepconstants::sup_file_extension_img_format_map.begin();
				 it != wlepconstants::sup_file_extension_img_format_map.end();
				 it++) {
				keys.push_back(it->first);
			}
			return keys;
		}

		/**
			Return a file extension of a given file
		*/
		static inline std::string getFileExtension(const std::string &filename) {
			const std::string::size_type idx = filename.rfind('.');

			if (idx != std::string::npos) {
				return filename.substr(idx + 1);
			}

			return "";
		}

		static inline std::wstring getFileExtension(const std::wstring &filename) {
			const std::wstring::size_type idx = filename.rfind('.');

			if (idx != std::wstring::npos) {
				return filename.substr(idx + 1);
			}

			return L"";
		}

		/**
			Find all files in a current directory and save them to @param files.

			@param directory - Path to desired directory
			@param files - Where to save the found filenames
			@param file_extensions - Find only files that have the specified extentions
								   - If empty, ignore file extensions
			@param recursive - Find files in all subfolders
		*/
		static void findFiles(const std::wstring &directory, std::vector<std::wstring> &files,
							  const std::vector<std::wstring> &file_extensions, bool recursive) {
			std::wstring tmp = directory + L"\\*";
			WIN32_FIND_DATAW file;
			HANDLE search_handle = FindFirstFileW(tmp.c_str(), &file);
			if (search_handle != INVALID_HANDLE_VALUE) {
				std::vector<std::wstring> directories;
				do {
					if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						// Ignore . and .. directories
						if (!lstrcmpW(file.cFileName, L".") || !lstrcmpW(file.cFileName, L"..")) {
							continue;
						}
					} else {
						if (!file_extensions.empty()) {
							const std::wstring extension = wleputils::FileUtil::getFileExtension(file.cFileName);
							auto it = std::find(file_extensions.begin(), file_extensions.end(), extension);

							if (it == file_extensions.end()) {
								continue;
							}
						}
						// If file_extensions are empty, get all files
						tmp = directory + L"\\" + std::wstring(file.cFileName);
						files.push_back(tmp);
					}
					tmp = directory + L"\\" + std::wstring(file.cFileName);
					if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						directories.push_back(tmp);
					}
				} while (FindNextFileW(search_handle, &file));

				if (!FindClose(search_handle)) {
					wleputils::ExceptionUtil::throwAndPrintException
						<std::exception>("Error while closing handle used for finding files!");
				}
				if (recursive) {
					for (auto it = directories.begin(), end = directories.end(); it != end; it++) {
						findFiles(*it, files, file_extensions, recursive);
					}
				}
			}
		}

		/**
			Check if a given directory exists
		*/
		static inline bool directoryExistsW(const std::wstring &path) {
			DWORD attr = GetFileAttributesW(path.c_str());
			return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
		}

		static inline std::string getFileNameWithoutExtension(const std::string &filename) {
			const std::string::size_type idx = filename.find('.');

			if (idx != std::string::npos) {
				return filename.substr(0, idx);
			}

			return "";
		}

		/*
			Once write fails, all further operations on the stream are no-ops, until the error is cleared.
			Including tellp, which is required to return -1 if any previous operation has failed.
		*/

		/**
			Write any vector to a file
			Return the bytes written
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

		/**
			Write any array to a file
			Return the bytes written
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

		/**
			Write any single item to a file
			Return the bytes written
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

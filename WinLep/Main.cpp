#define TIME
#include <iostream>
#include <string>

#ifdef TIME
#include <ctime>
#endif // TIME

#include "wlep_writer.h"
#include "wlep_image.h"
#include "file_util.h"
#include "directory.h"

/*
	This code has been written according to Google's C++ style standards.
	If you were to contribute to this, please use this coding style.
	https://google.github.io/styleguide/cppguide.html

	Since C++11, you may want to use either snake_case or camelCase for function names.
	This is because to make a class work as the range-expression in a range-based for-loop,
	you have to define functions called begin and end (case-sensitive) for that class.
	https://stackoverflow.com/questions/1776291/function-names-in-c-capitalize-or-not

	Therefore for function names prefer camelCase instead of PascalCase.
*/

void printHelp();

void printWinLepVersion() {
	std::cerr << "WinLep v" << std::to_string(wlepconstants::version[0]) << '.'
		<< std::to_string(wlepconstants::version[1]) << '.'
		<< std::to_string(wlepconstants::version[2]) << '\n';
}

#ifdef TIME
double diffClock(clock_t clock1, clock_t clock2) {
	double diffticks = clock1 - (double)clock2;
	double diffms = (diffticks) / (CLOCKS_PER_SEC / 1000);
	return diffms;
}
#endif // TIME

void convertAndWriteFiles(std::vector<std::string> &in_filenames, std::vector<std::string> &out_filenames) {
	if (in_filenames.empty() || out_filenames.empty()) {
		wleputils::ExceptionUtil::printErrorMsg("Provided filenames are empty!");
		return;
	}

	// Both vectors should have the same size 
	// since every input filename should have a corresponding output filename
	if (in_filenames.size() != out_filenames.size()) {
		wleputils::ExceptionUtil::printErrorMsg("Provided filename vectors doesn't have the same size!");
		return;
	}

	for (int i = 0; i < in_filenames.size(); i++) {
		wlep::WLepWriter writer(out_filenames[i], in_filenames[i]);
#ifdef TIME
		clock_t start = std::clock();
#endif // TIME
		size_t bytes_written = writer.writeWinLepFile();
#ifdef TIME
		clock_t end = std::clock();
		std::cerr << "[TIME] Writing  " << out_filenames[i]
			<< " and converting it to lepton took " << diffClock(end, start) << "ms\n";
#endif // TIME
		std::cerr << "[INFO] Converted " << in_filenames[i] << " --> "
			<< out_filenames[i] << " (" << bytes_written / 1000.0f << "kB)\n\n"; 
	}
}

std::string processInputFilename(char *filename) {
	std::string result(filename);
	wleputils::StringUtil::toLowerCase(result);

	auto it = wlepconstants::sup_file_extension_img_format_map.begin();
	bool is_supported_extension = false;

	// Check if the input file ends with one of the supported file extensions
	while (it != wlepconstants::sup_file_extension_img_format_map.end()) {
		std::string sup_file_extension = "";
		sup_file_extension += '.' + it->first;
		is_supported_extension |= wleputils::StringUtil::endsWith(result, sup_file_extension);

		it++;
	}

	if (!is_supported_extension) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::invalid_argument>("Invalid input file!", "Supported files are: .jpg/.jpeg");
	}

	return result;
}

std::string processOutputFilename(char *filename) {
	std::string str_filename(filename);
	std::string extension = wleputils::FileUtil::getFileExtension(str_filename);

	if (extension.empty()) {
		return str_filename + wlepconstants::file_extension;
	}

	wleputils::StringUtil::toLowerCase(extension);

	if (extension != wlepconstants::file_extension) {
		return str_filename + wlepconstants::file_extension;
	}

	return str_filename;
}

int main(int argc, char **argv) {
#ifndef _WIN32
	std::cerr << "[ERROR] winLep is implemented only for Windows OS! (Hence the name ...)\n";
	return -1;
#endif // !_WIN32

	if (argc <= 1) {
		printHelp();
		return 0;
	}
	printWinLepVersion();

	std::vector<std::string> in_filenames;
	std::vector<std::string> out_filenames;

	for (int i = 1; i < argc; i++) {
		// Options
		if (argv[i][0] == '-') {
			if (strcmp(argv[i], "-help") == 0) {
				printHelp();
				return 0;
				// Convert all .jpg/.jpeg images in the given directory
			} else if (strcmp(argv[i], "-d") == 0) {
				// No directory name
				if (argc < i + 1) {
					wleputils::ExceptionUtil::printErrorMsg("Enter a directory name!");
					printHelp();
					return -1;
				}
				std::string output_dir = "";
				// [output directory] is provided
				if (argc > (i + 2)) {
					output_dir = argv[i + 2];
					if (output_dir[output_dir.length() - 1] != '\\') {
						output_dir += '\\';
					}
				}
				wlep::Directory *dir = new wlep::Directory(argv[i + 1]);
				in_filenames = dir->getAllFiles({"jpg", "jpeg"});

				for (std::string filename : in_filenames) {
					out_filenames.push_back(
						output_dir
						+ wleputils::FileUtil::getFileNameWithoutExtension(filename)
						+ wlepconstants::file_extension);
				}
				delete dir;
				break;
				// Convert all .jpg/.jpeg images in given directory and all of its subdirectories
			} else if (strcmp(argv[i], "-D") == 0) {
				// No directory name
				if (argc < i + 1) {
					wleputils::ExceptionUtil::printErrorMsg("Enter a directory name!");
					printHelp();
					return -1;
				}

				std::string output_dir = "";
				// [output directory] is provided
				if (argc > (i + 2)) {
					output_dir = argv[i + 2];
					if (output_dir[output_dir.length() - 1] != '\\') {
						output_dir += '\\';
					}
				}

				wlep::Directory *dir = new wlep::Directory(argv[i + 1], true); 
				in_filenames = dir->getAllFiles({"jpg", "jpeg"});

				for (std::string &filename : in_filenames) {
					out_filenames.push_back(
						output_dir
						+ wleputils::FileUtil::getFileNameWithoutExtension(filename)
						+ wlepconstants::file_extension);
				}
				delete dir;
				break;
			} else {
				std::string msg = "Unknown option " + std::string(argv[i]);
				wleputils::ExceptionUtil::printErrorMsg(msg);
				printHelp();
				return -1;
			}
			// Convert just one file
		} else {
			in_filenames.push_back(processInputFilename(argv[i]));
			if (argc > 2) {
				out_filenames.push_back(processOutputFilename(argv[i + 1]));
			} else {
				out_filenames.push_back(wleputils::FileUtil::getFileNameWithoutExtension(in_filenames[0]) + wlepconstants::file_extension);
			}
			break;
		}
	}

	convertAndWriteFiles(in_filenames, out_filenames);

	return 0;
}

void printHelp() {
	// TODO: Put the help menu into a text file and just print it.
	printWinLepVersion();
	// General info
	std::cerr << "Make sure lepton.exe is in your PATH\n\n";
	std::cerr << "Usage: WinLep <input_file> [output_file] [options]\n";
	std::cerr << "If no output file is provided, input filename will be used, with the extension " << wlepconstants::file_extension << '\n';
	std::cerr << "If no output file extension is provided, or other than " << wlepconstants::file_extension
		<< ", " << wlepconstants::file_extension << " will be used automatically.\n";
	std::cerr << "\nOptions:\n\t-help:\tShow this menu\n";

	// Options
	std::cerr << "\t-d <directory> [output_directory]: - Converts all .jpg/.jpeg files in the given directory" <<
		"\n\t\t\tto [output_directory]. Orginal filenames will be used, with the .wlep extension" <<
		"\n\t\t\tIf no [output_directory] is provided, the files will be outputted to [directory]." <<
		"\n\t\t\tIf [output_directory] is provided and it doesn't exists, it's created.\n";

	std::cerr << "\t-D <directory> [output_directory]: - Converts all .jpg/.jpeg files in the given directory" <<
		"\n\t\t\tand all of its subdirectories to [output_directory]." <<
		"\n\t\t\tOrginal filenames will be used with the added .wlep extension." <<
		"\n\t\t\tIf no [output_directory] is provided, the files will be outputted to [directory]" <<
		"\n\t\t\tIf [output_directory] is provided and it doesn't exists, it's created" <<
		"\n\t\t\talong with all other subdirectories." << 
		"\n\t\t\tAll original subdirectory names are preserved and created in the same structure\n";

	// Examples
	std::cerr << "Examples:\n\tWinLep test.jpg --> Converts test.jpg and saves it as test" << wlepconstants::file_extension
		<< "\n\tWinLep test.jpg picture --> Converts test.jpg and saves it as picture" << wlepconstants::file_extension
		<< "\n\tWinLep test.jpg out" << wlepconstants::file_extension
		<< " --> Converts test.jpg and saves it as out" << wlepconstants::file_extension
		<< "\n\tWinLep -d . --> Converts all the .jpg/.jpeg images in the current directory to " << wlepconstants::file_extension
		<< "\n\tWinLep -d . wlep_images --> Converts all the .jpg/.jpeg images in the current directory"
		<< "\n\t\t\tand saves them into wlep_images folder."
		<< "\n\tWinLep -D . --> Converts all .jpg/.jpeg images in the current directory and all of its subdirectories"
		<< "\n\t\t\tConverted files are created in the same subfolders as the original ones."
		<< "\n\tWinLep -D . test --> Converts all .jpg/.jpeg images in the current folder and writes them in"
		<< "\n\t\t\tthe subfolder 'test' creating the original subfolder structure.";
}
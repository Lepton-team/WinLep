//#define DEBUG 
#define SAVE_THUMBNAIL

#include <iostream>
#include <vector>
#include <string>

#include "wlep_reader.h"
#include "wlep_writer.h"
#include "wlep_image.h"
#include "file_util.h"
#include "directory.h"

#ifdef DEBUG
std::string out_filename = "out.wlep";
std::string test_lep_filename = "test.wlep";
std::string test_jpg_filename = "test.jpg";
#endif // DEBUG


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

void convertAndWriteFiles(std::vector<std::string> &in_filenames, std::vector<std::string> &out_filenames) {
	if (in_filenames.empty() || out_filenames.empty()) {
		wleputils::ExceptionUtil::printErrorMsg("Provided filenames are empty!");
		return;
	}

	// Both vectors should have the same size
	if (in_filenames.size() != out_filenames.size()) {
		wleputils::ExceptionUtil::printErrorMsg("Provided filename vectors doesn't have the same size!");
		return;
	}

	for (int i = 0; i < in_filenames.size(); i++) {
		wlep::WLepWriter writer(out_filenames[i], in_filenames[i]);
		writer.writeWinLepFile();
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
	std::string result(filename);
	wleputils::StringUtil::toLowerCase(result);

	if (!wleputils::StringUtil::endsWith(result, wlepconstants::file_extension)) {
		result += wlepconstants::file_extension;
	}

	return result;
}

int main(int argc, char **argv) {
#ifndef _WIN32
	std::cerr << "[ERROR] winLep is implemented only for Windows OS! (Hence the name ...)\n";
	return -1;
#endif // !_WIN32

	std::cout << "WinLep v" << std::to_string(wlepconstants::version[0]) << '.'
		<< std::to_string(wlepconstants::version[1]) << '.'
		<< std::to_string(wlepconstants::version[2]) << '\n';

	if (argc <= 1) {
		printHelp();
		return 0;
	}

	printHelp();

	std::vector<std::string> in_filenames;
	std::vector<std::string> out_filenames;

	for (int i = 1; i < argc; i++) {
		// Options
		if (argv[i][0] == '-') {
			if (strcmp(argv[i], "-help") == 0) {
				printHelp();
				return 0;
				// Convert all .jpg/.jpeg images in given directory
			} else if (strcmp(argv[i], "-d") == 0) {
				// No directory name
				if (argc <= 2) {
					wleputils::ExceptionUtil::printErrorMsg("Enter a directory name!");
					printHelp();
					return -1;
				}

				std::string output_dir = "";
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
				// TODO: For now ?
				break;
				// Convert all .jpg/.jpeg images in given directory and all of its subdirectories
			} else if (strcmp(argv[i], "-D") == 0) {
				// TODO: Implement
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
				if (argv[i + 1][0] == '-') {
					continue;
				}
				out_filenames.push_back(processOutputFilename(argv[i + 1]));
			} else {
				out_filenames.push_back(wleputils::FileUtil::getFileNameWithoutExtension(in_filenames[0]) + wlepconstants::file_extension);
			}
			break;
		}
	}

	convertAndWriteFiles(in_filenames, out_filenames);

	std::cout << "Done.\n";

	return 0;
}

void printHelp() {
	// General info
	std::cerr << "Please make sure lepton.exe is present in the same directory as WinLep. \n";
	std::cerr << "Usage: WinLep [input file] [output file] -[options]\n";
	std::cerr << "If no output file is provided, input filename will be used, with the extension " << wlepconstants::file_extension << '\n';
	std::cerr << "If no output file extension is provided, or other than " << wlepconstants::file_extension
		<< ", " << wlepconstants::file_extension << " will be used automatically.\n";
	std::cerr << "\nOptions:\n\t-help:\tShow this menu\n";

	// Options
	std::cerr << "\t-d [directory] [output directory]: - Converts all .jpg/.jpeg files in given directory" <<
		"\n\t\t\tto the output directory. Orginal filenames will be used, with the .wlep extension" <<
		"\n\t\t\tIf no output directory is provided, the files will be outputted to [directory]." <<
		"\n\t\t\tIf output directory is provided, it must exist.\n";

	std::cerr << "\t-D [directory] [output directory]: - Converts all .jpg/.jpeg files in given directory" <<
		"\n\t\t\tand all of its subdirectories to the output directory." <<
		"\n\t\t\tOrginal filenames will be used, with the .wlep extension." <<
		"\n\t\t\tIf no output directory is provided, the files will be outputted to [directory]" <<
		"\n\t\t\tIf output directory is provided, it must exist.\n";

	// Examples
	std::cerr << "\nExamples:\n\tWinLep test.jpg out" << wlepconstants::file_extension
		<< " --> Converts test.jpg and saves it as out" << wlepconstants::file_extension << '\n';
	std::cerr << "\tWinLep -d . --> Converts all the .jpg/.jpeg images in the current directory to " << wlepconstants::file_extension << '\n';
	std::cerr << "\tWinLep -d . wlep_images --> Converts all the .jpg/.jpeg images in the current directory" <<
		"\n\t\t\t\tand saves them into wlep_images folder. \n";


}
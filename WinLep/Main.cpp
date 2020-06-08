#define TIME
#include <iostream>

#include <ctime>

#include "wlep_writer.h"
#include "wlep_reader.h"
#include "wlep_image.h"
#include "file_util.h"
#include "directory.h"
#include "input_parser.h"

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

bool g_convert_to_jpg = false;
bool g_delete_original = false;
bool g_verbose = false;
void printHelp();

void printWinLepVersion() {
	std::cerr << "WinLep v" << std::to_string(wlepconstants::version[0]) << '.'
		<< std::to_string(wlepconstants::version[1]) << '.'
		<< std::to_string(wlepconstants::version[2]) << '\n';
}

double diffClock(clock_t clock1, clock_t clock2) {
	double diffticks = clock1 - (double)clock2;
	double diffms = (diffticks) / (CLOCKS_PER_SEC / 1000);
	return diffms;
}

size_t convertAndWriteFiles(std::vector<std::string> &in_filenames, std::vector<std::string> &out_filenames) {
	// Both vectors should have the same size 
	// since every input filename should have a corresponding output filename
	if (in_filenames.size() != out_filenames.size()) {
		wleputils::ExceptionUtil::printErrorMsg("Provided filename vectors doesn't have the same size!");
		return 0;
	}

	size_t bytes_written = 0;
	size_t total_bytes_written = 0;
	for (int i = 0; i < in_filenames.size(); i++) {
		if (g_verbose) {
			std::cerr << "[INFO] Converting " << in_filenames[i] << " . . .\n";
		}

		clock_t start = std::clock();
		if (g_convert_to_jpg) {
			wlep::WLepReader reader(in_filenames[i]);
			std::vector<uChar> lepton_data = reader.validateFileAndReadLeptonData();
			wlep::WLepWriter writer(in_filenames[i], out_filenames[i], false);
			bytes_written = writer.writeJpgFile(lepton_data);
		} else {
			wlep::WLepWriter writer(out_filenames[i], in_filenames[i]);
			bytes_written = writer.writeWinLepFile();
		}

		clock_t end = std::clock();
		total_bytes_written += bytes_written;
		std::cerr << "[INFO] Sucessfully converted " << in_filenames[i] << " --> " << out_filenames[i]
			<< " (" << bytes_written / 1000.0f << "kB) [" << diffClock(end, start) << "ms]\n";

		if (g_delete_original) {
			remove(in_filenames[i].c_str());
			std::cerr << "[INFO] Removed " << in_filenames[i] << '\n';
		}
	}

	return total_bytes_written;
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

void setupFilenames(const std::string &output_dir, const std::string &input_dir,
					std::vector<std::string> &in_filenames, std::vector<std::string> &out_filenames, bool recursive) {

	wlep::Directory *dir = new wlep::Directory(input_dir, recursive);
	if (g_convert_to_jpg) {
		in_filenames = dir->getAllFiles({"wlep"});
	} else {
		in_filenames = dir->getAllFiles({"jpg", "jpeg"});
	}
	std::string out_extension = g_convert_to_jpg ? wlepconstants::jpg_extension : wlepconstants::file_extension;

	if (g_verbose) {
		std::cerr << "[INFO] Found " << in_filenames.size() << " files to convert:";
		for (const std::string &file : in_filenames) {
			std::cerr << "\n\t" << file;
		}
		std::cerr << "\n";
	}

	for (std::string filename : in_filenames) {
		auto idx = filename.find_first_of('\\');
		filename.replace(0, idx + 1, output_dir);
		out_filenames.push_back(
			wleputils::FileUtil::getFileNameWithoutExtension(filename)
			+ out_extension);
	}

	delete dir;
}
std::string setupOutputDir(const std::string &dir) {
	std::string output_dir = dir;
	if (output_dir[output_dir.length() - 1] != '\\') {
		output_dir += '\\';
	}
	if (g_verbose) {
		std::cerr << "[INFO] Path to output directory: " << output_dir << "\n";
	}
	return output_dir;
}

int main(int argc, char **argv) {
#ifndef _WIN32
	std::cerr << "[ERROR] WinLep is implemented only for Windows OS! (Hence the name ...)\n";
	return -1;
#endif // !_WIN32

	if (argc <= 1) {
		printHelp();
		return 0;
	}
	printWinLepVersion();

	std::vector<std::string> in_filenames;
	std::vector<std::string> out_filenames;

	std::string output_dir = "";
	std::string input_dir = "";
	bool is_output_dir_provided = false;

	wlep::InputParser input(argc, argv);

	if (input.cmdOptionExists("-help") || input.cmdOptionExists("-h")) {
		printHelp();
		return 0;
	}

	if (input.cmdOptionExists("-v") || input.cmdOptionExists("-verbose")) {
		g_verbose = true;
	}

	if (input.cmdOptionExists("-r")) {
		g_delete_original = true;
	}

	if (input.cmdOptionExists("-j")) {
		// Convert wlep to jpg
		g_convert_to_jpg = true;
	} 

	if (input.cmdOptionExists("-J")) {
		// Convert wlep to jpg and delete original jpgs
		g_convert_to_jpg = true;
		g_delete_original = true;
	} 

	if (input.cmdOptionExists("-w")) {
		// Convert jpg to wlep
		g_convert_to_jpg = false;
	} 

	if (input.cmdOptionExists("-W")) {
		// Convert jpg to wlep and delete original wleps
		g_convert_to_jpg = false;
		g_delete_original = true;
	} 

	if (input.cmdOptionExists("-d")) {
		// Convert whole directory
		input_dir = input.getFirstCmdOption("-d");
		if (input_dir.empty()) {
			wleputils::ExceptionUtil::printErrorMsg("Enter a directory name!");
			printHelp();
			return -1;
		}

		output_dir = setupOutputDir(input.getSecondCmdOption("-d"));
		setupFilenames(output_dir, input_dir, in_filenames, out_filenames, false);
	} else if (input.cmdOptionExists("-D")) {
		// Convert whole directory and all of it's subdirectories
		input_dir = input.getFirstCmdOption("-D");
		if (input_dir.empty()) {
			wleputils::ExceptionUtil::printErrorMsg("Enter a directory name!");
			printHelp();
			return -1;
		}

		output_dir = setupOutputDir(input.getSecondCmdOption("-D"));
		setupFilenames(output_dir, input_dir, in_filenames, out_filenames, true);
	} else {
		wleputils::ExceptionUtil::printErrorMsg("Unknown option");
		printHelp();
		return -1;
	}

	//			// Convert all .jpg/.jpeg images in given directory and all of its subdirectories
	//		} else if (strcmp(argv[i], "-D") == 0) {
	//			// No directory name
	//			if (argc < i + 1) {
	//				wleputils::ExceptionUtil::printErrorMsg("Enter a directory name!");
	//				printHelp();
	//				return -1;
	//			}
	//			// [output directory] is provided
	//			if (argc > (i + 2)) {
	//				output_dir = setupOutputDir(argv[i + 2]);
	//				is_output_dir_provided = true;
	//			}
	//			setupFilenames(output_dir, argv[i + 1], in_filenames, out_filenames, true);
	//			// Skip the next 1 or 2 args
	//			i += is_output_dir_provided ? 2 : 1;
	//			continue;
	//		} else {
	//			std::string msg = "Unknown option " + std::string(argv[i]);
	//			wleputils::ExceptionUtil::printErrorMsg(msg);
	//			printHelp();
	//			return -1;
	//		}
	//		// Convert just one file
	//	} else {
	//		in_filenames.push_back(processInputFilename(argv[i]));
	//		if (argc > 2) {
	//			out_filenames.push_back(processOutputFilename(argv[i + 1]));
	//		} else {
	//			out_filenames.push_back(wleputils::FileUtil::getFileNameWithoutExtension(in_filenames[0]) + wlepconstants::file_extension);
	//		}
	//		break;
	//	}
	//}
	clock_t start = std::clock();

	if (in_filenames.empty() || out_filenames.empty()) {
		std::string msg = "No " + g_convert_to_jpg ? wlepconstants::file_extension : "JPEG";
		msg += " images found in " + input_dir;
		wleputils::ExceptionUtil::printErrorMsg(msg);
		return -1;
	}

	size_t total_bytes_written = convertAndWriteFiles(in_filenames, out_filenames);

	clock_t end = std::clock();
	double total_time = diffClock(end, start);

	std::string msg = g_delete_original
		? "[TOTAL] Wrote, converted and removed "
		: "[TOTAL] Wrote and converted ";

	std::cerr << msg << in_filenames.size()
		<< " images (" << total_bytes_written / 1000.0f << "kB) [" << total_time << "ms]\n";

	return 0;
}

void printHelp() {
	// TODO: Put the help menu into a text file and just print it.
	printWinLepVersion();
	// General info
	std::cerr << "Make sure lepton.exe is in your PATH\n";
	// Name
	std::cerr << "\nNAME" << "\n\tWinLep - Convert JPEG images to .wlep and vice versa";
	// Synopsis
	std::cerr << "\nSYNOPSIS\n\tWinLep <input_file> [output_file] [options]";
	std::cerr << "\n\tWinLep [options] [-d] [-D] <directory> [output_directory]";

	// Description
	std::cerr << "\nDESCRIPTION";
	std::cerr << "\n\tWinLep is a windows exclusive command line application used for converting "
		<< "\n\tJPEG images to " << wlepconstants::file_extension << " format and vice versa. "
		<< "The biggest benefit of " << wlepconstants::file_extension 
		<< "\n\tformat is the file size. In average it's 22% smaller than original JPEG."
		<< "\n\tThe compression/decompression is lossless and the original file is preserved bit-for-bit perfeclty";
	//std::cerr << "If no output file is provided, input filename will be used, with the extension "
	//	<< wlepconstants::file_extension << '\n';
	//std::cerr << "If no output file extension is provided, or other than " << wlepconstants::file_extension
	//	<< ", " << wlepconstants::file_extension << " will be used automatically.\n";
	std::cerr << "\nOPTIONS\n\t-h -help \n\t\t- Show this menu\n";

	// Options
	std::cerr << "\t-d <directory> [output_directory]" <<
		"\n\t\t- Converts all JPEG images in the given directory" <<
		"\n\t\tto [output_directory]. Original filenames will be used, with the .wlep extension" <<
		"\n\t\tIf no [output_directory] is provided, the files will be outputted to [directory]." <<
		"\n\t\tIf [output_directory] is provided and it doesn't exist, it's created.\n";

	std::cerr << "\t-D <directory> [output_directory]" <<
		"\n\t\t- Converts all JPEG images in the given directory" <<
		"\n\t\tand all of its subdirectories to [output_directory]." <<
		"\n\t\tOriginal filenames will be used with the added .wlep extension." <<
		"\n\t\tIf no [output_directory] is provided, the files will be outputted to [directory]" <<
		"\n\t\tIf [output_directory] is provided and it doesn't exist, it's created" <<
		"\n\t\talong with all other subdirectories." <<
		"\n\t\tAll original subdirectory names are preserved and created in the same structure\n";

	std::cerr << "\t-r\t- Removes original images\n";
	std::cerr << "\t-j\t- Converts desired file/directory from " << wlepconstants::file_extension
		<< " to " << wlepconstants::jpg_extension << '\n';

	std::cerr << "\t-w\t- (default) Converts desired file/directory from JPEG to "
		<< wlepconstants::file_extension << '\n';

	std::cerr << "\t-J\t- Converts desired file/directory from " << wlepconstants::file_extension
		<< " to " << wlepconstants::jpg_extension
		<< "\n\t\tand REMOVES all original " << wlepconstants::file_extension << " images. Same as -j -r\n";

	std::cerr << "\t-W\t- Converts desired file/directory from " << wlepconstants::jpg_extension
		<< " to " << wlepconstants::file_extension
		<< "\n\t\tand REMOVES all original JPEG images. Same as -w -r\n";
	
	std::cerr << "\t-v -verbose\n\t\t- Outputs more information to the console";

	// Examples
	std::cerr << "\nEXAMPLES\n\tWinLep test.jpg --> Converts test.jpg and saves it as test" << wlepconstants::file_extension
		<< "\n\tWinLep test.jpg picture --> Converts test.jpg and saves it as picture" << wlepconstants::file_extension
		<< "\n\tWinLep -w test.jpg out" << wlepconstants::file_extension
		<< " --> Converts test.jpg and saves it as out" << wlepconstants::file_extension
		<< "\n\tWinLep -j -d . --> Converts all the " << wlepconstants::file_extension
		<< " images in the current directory to " << wlepconstants::jpg_extension

		<< "\n\tWinLep -d . wlep_images --> Converts all the JPEG images in the current directory"
		<< "\n\t\t\tand saves them into wlep_images folder."
		<< "\n\tWinLep -w -D . --> Converts all JPEG images in the current directory and all of its subdirectories"
		<< "\n\t\t\tConverted files are created in the same subfolders as the original ones."
		<< "\n\tWinLep -j -D . pics --> Converts all " << wlepconstants::file_extension << " images in the current folder to "
		<< wlepconstants::jpg_extension << " and writes them in"
		<< "\n\t\t\tthe subfolder 'pics' creating the original subfolder structure.";

	// Bugs
	std::cerr << "\nBUGS";
	std::cerr << "\n\t";

	// Author
	std::cerr << "\nAUTHOR";
	std::cerr << "\n\tMarek Oksa, xoksamarek@gmail.com\n";

}
#define DEBUG 
#define SAVE_THUMBNAIL

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include "wlep_reader.h"
#include "wlep_writer.h"
#include "wlep_image.h"
#include "directory.h"

std::string out_filename = "out.wlep";
std::string test_lep_filename = "test.wlep";
std::string test_jpg_filename = "test.jpg";

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
void testReading() {
	wlep::WLepReader reader(out_filename);
	reader.readHeader();

	std::cout << reader.debug_str() << '\n';
}

void testWriting() {
	try {
		wlep::WLepWriter writer(out_filename, test_jpg_filename);
		writer.writeWinLepFile();
		std::cout << writer.debug_str() << '\n';
	} catch (...) {

	}
}

int main(int argc, char **argv) {
#ifndef _WIN32
	std::cerr << "[ERROR] winLep is implemented only for Windows OS! (Hence the name ...)\n";
	return -1;
#endif // !_WIN32

	testWriting();
	testReading();
	DWORD file_attr = GetFileAttributesW(L".");

	if (file_attr == INVALID_FILE_ATTRIBUTES) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::exception>("Error while reading file attributes!");
	}

	if (file_attr == FILE_ATTRIBUTE_NORMAL) {
		// TODO: Convert
		std::cerr << "Normal\n";
	}

	if (file_attr == FILE_ATTRIBUTE_DIRECTORY) {
		// TODO: Find all .jpg files and covert them to .wlep
		std::cerr << "Directory\n";
		wlep::Directory *dir = new wlep::Directory(".");
		std::vector<std::string> files = dir->getAllFiles("jpg");

		for (std::string file : files) {
			std::cerr << file << "\n";
		}

		delete dir;
	}

	std::cout << "Done.\n";

	return 0;
}
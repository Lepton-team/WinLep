#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <Windows.h>
#include "ProcessUtil.h"

#include "WLepReader.h"
#include "WLepWriter.h"

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
	wlep::WLepWriter writer(out_filename, test_jpg_filename);
	writer.writeHeader();

	std::cout << writer.debug_str() << '\n';
}

int main() {
	testWriting();
	testReading();

	PROCESS_INFORMATION lepton_process = wlep::ProcessUtil::launchProcess("lepton.exe", "");

	if (!wlep::ProcessUtil::isProcessActive(lepton_process)) {
		wlep::ProcessUtil::stopProcess(lepton_process);
	};

	return 0;
}
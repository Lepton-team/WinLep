#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <jpgcoder.hh>

#include "WLepConstants.h"
#include "WLepHeader.h"
#include "WLepReader.h"
#include "WLepWriter.h"

std::string out_filename = "out.wlep";
std::string test_lep_filename = "test.wlep";
std::string test_jpg_filename = "test.jpg";

template<typename T>
void print_arr_hex(T *arr, const int size) {
	for (int i = 0; i < size; i++) {
		printf("%x ", arr[i]);
	}

	std::cout << "\n";
}

void test_reading() {
	WLep::WLepReader reader(out_filename);
	reader.read_header();

	std::cout << reader.debug_str() << '\n';
}

void test_writing() {
	WLep::WLepWriter writer(out_filename, test_jpg_filename);
	writer.write_header();

	std::cout << writer.debug_str() << '\n';
}

int main() {
	test_writing();
	test_reading();

	return 0;
}
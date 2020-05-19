#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <jpgcoder.hh>

#include "WLepConstants.h"
#include "WLepHeader.h"
#include "WLepUtils.h"
#include "WLepReader.h"

const std::string out_filename = "out.wlep";
const std::string test_lep_filename = "test.wlep";
const std::string test_jpg_filename = "test.jpg";

template<typename T>
void print_arr_hex(T *arr, const int size) {
	for (int i = 0; i < size; i++) {
		printf("%x ", arr[i]);
	}

	std::cout << "\n";
}

void test_reading() {
	WLep::WLepReader reader(out_filename);
	reader.read_all(false);

	std::cout << reader.debug_str();
}

int main() {
	WLep::WLepHeader wlep_header = WLep::WLepHeader(test_jpg_filename);
	std::vector<uChar> data = std::vector<uChar>();

	uFstream file;
	// Write
	file.open(out_filename, std::ios::out | std::ios::binary);

	FileUtil::write_to_file<uChar>(file, wlep_header.header_prefix);
	FileUtil::write_to_file<uChar>(file, wlep_header.version);
	FileUtil::write_to_file<uChar>(file, wlep_header.thumbnail_size_arr);
	FileUtil::write_to_file<uChar>(file, wlep_header.thumbnail_data);

	std::string hex_str = "";

	for (int i = 0; i < WLepConstants::thumbnail_size_size; i++) {
		std::stringstream stream;
		stream << std::hex << int(wlep_header.thumbnail_size_arr[i]);
		hex_str += stream.str();
	}

	std::istringstream converter(hex_str);
	size_t value;
	converter >> std::hex >> value;

	std::cout << "\nWritten thumbnail size: " << value << " B -> 0x" << hex_str << "\n";

	file.close();

	// Test
	test_reading();

	return 0;
}
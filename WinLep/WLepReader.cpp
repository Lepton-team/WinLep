#include "WLepReader.h"

void WLep::WLepReader::read_header() {
	if (!file.is_open()) {
		file.open(this->filename, std::ios::in | std::ios::binary);
	}

	// Header prefix
	read_from_file_to_vec(this->header.header_prefix, WLepConstants::header_prefix_size);

	// Version
	read_from_file_to_vec(this->header.version, WLepConstants::version_size);

	// Thumbnail size
	read_from_file_to_vec(this->header.thumbnail_size_arr, WLepConstants::thumbnail_size_size);
	this->header.thumbnail_size = WLepUtils::VectorUtil::hex_vec_to_int(this->header.thumbnail_size_arr);
}

void WLep::WLepReader::close_fstream() {
	if (file.is_open()) {
		file.close();
	}
}

void WLep::WLepReader::read_from_file_to_vec(std::vector<uChar> &vec, size_t bytes) {
	uChar *buf = new uChar[bytes];
	file.read(buf, bytes);
	WLepUtils::VectorUtil::to_vector(vec, buf, bytes);

	delete[] buf;
}

std::string WLep::WLepReader::debug_str() {
	std::string res = "";

	res.append("\nRead header prefix: ")
		.append(WLepUtils::VectorUtil::hex_vec_to_hex_str(this->header.header_prefix))
		.append("\nRead version: ")
		.append(WLepUtils::VectorUtil::hex_vec_to_hex_str(this->header.version))
		.append("\nRead thumbnail size: ")
		.append(std::to_string(this->header.thumbnail_size))
		.append(" B\t-> 0x")
		.append(WLepUtils::VectorUtil::hex_vec_to_hex_str(this->header.thumbnail_size_arr));

	return res;
}

WLep::WLepReader::WLepReader(const std::string const &filename)
	: filename(filename) {
	file.open(filename, std::ios::in | std::ios::binary);

	// Stop eating new lines in binary mode!!!
	file.unsetf(std::ios::skipws);

	header = WLep::WLepHeader();
}

WLep::WLepReader::~WLepReader() {
	close_fstream();
}
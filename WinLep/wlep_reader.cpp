#include "wlep_reader.h"

void wlep::WLepReader::readHeader() {
	if (!this->file_.is_open()) {
		this->file_.open(this->filename_, std::ios::in | std::ios::binary);
	}

	// Header prefix
	readFromFileToVector(this->header.header_prefix, wlepconstants::header_prefix_size);

	// Version
	readFromFileToVector(this->header.version, wlepconstants::version_size);

	// Thumbnail size
	readFromFileToVector(this->header.thumbnail_size_arr, wlepconstants::thumbnail_size_size);
	this->header.thumbnail_size = wleputils::VectorUtil::hexVectorToInt(this->header.thumbnail_size_arr);
}

void wlep::WLepReader::closeFileStream() {
	if (this->file_.is_open()) {
		this->file_.close();
	}
}

void wlep::WLepReader::readFromFileToVector(std::vector<uChar> &vec, size_t bytes) {
	uChar *buf = new uChar[bytes];
	this->file_.read(buf, bytes);
	wleputils::VectorUtil::arrayToVector(vec, buf, bytes);

	delete[] buf;
}

std::string wlep::WLepReader::debug_str() {
	std::string res = "";

	res.append("\nRead header prefix: ")
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.header_prefix))
		.append("\nRead version: ")
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.version))
		.append("\nRead thumbnail size: ")
		.append(std::to_string(this->header.thumbnail_size))
		.append(" B\t-> 0x")
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.thumbnail_size_arr));

	return res;
}

wlep::WLepReader::WLepReader(const std::string const &filename)
	: filename_(filename) {
	if (filename.empty()) {
		throw std::invalid_argument("Filename cannot be empty!");
	}

	this->file_.open(filename, std::ios::in | std::ios::binary);

	// Stop eating new lines in binary mode!!!
	this->file_.unsetf(std::ios::skipws);

	header = wlep::WLepHeader();
}

wlep::WLepReader::~WLepReader() {
	closeFileStream();
}
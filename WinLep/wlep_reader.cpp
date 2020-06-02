#include "wlep_reader.h"
#include "vector_util.h"
#include "file_util.h"
#include "exception_util.h"
#include <iostream>

void wlep::WLepReader::readHeader() {
	// Header prefix
	readFromFileToVector(this->header.header_prefix, wlepconstants::header_prefix_size);

	// Version
	readFromFileToVector(this->header.version, wlepconstants::version_size);

	// Thumbnail size
	readFromFileToVector(this->header.thumbnail_size_arr, wlepconstants::thumbnail_size_size);
	this->header.thumbnail_size = wleputils::VectorUtil::hexVectorToInt(this->header.thumbnail_size_arr);
}

void wlep::WLepReader::readFromFileToVector(std::vector<uChar> &vec, size_t bytes) {
	if (this->filename_.empty() || !this->file_.good()) {
		wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Error while reading from file!");
	}
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

wlep::WLepReader::WLepReader(std::string const &filename)
	: filename_(filename) {

	wleputils::FileUtil::openFileStream(this->file_, this->filename_, std::ios::in);
	header = wlep::WLepHeader();
}

wlep::WLepReader::~WLepReader() {
	wleputils::FileUtil::closeFileStream(this->file_);
}
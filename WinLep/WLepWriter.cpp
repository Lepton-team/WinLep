#include "WLepWriter.h"

wlep::WLepWriter::WLepWriter(std::string &filename, wlep::WLepHeader &header)
	:header(header) {
	if (filename.empty()) {
		throw std::invalid_argument("Filename cannot be empty!");
	}

	if (wleputils::StringUtil::endsWith(filename, wlepconstants::file_extension)) {
		filename.append(wlepconstants::file_extension);
	}

	openFileStream(filename);

	this->filename_ = filename;
}

wlep::WLepWriter::WLepWriter(std::string &filename, const std::string &thumbnail_filename) {
	if (filename.empty() || thumbnail_filename.empty()) {
		throw std::invalid_argument("Filename cannot be empty!");
	}

	this->header = wlep::WLepHeader(thumbnail_filename);

	if (!wleputils::StringUtil::endsWith(filename, wlepconstants::file_extension)) {
		filename.append(wlepconstants::file_extension);
	}

	openFileStream(filename);

	this->filename_ = filename;
}

wlep::WLepWriter::~WLepWriter() {
	closeFileStream();
}

size_t wlep::WLepWriter::writeHeader() {
	size_t bytes_written = 0;
	const int items_to_write = 4;

	size_t bytes_written_items[items_to_write] = {
		wleputils::FileUtil::writeToFile<uChar>(file_, this->header.header_prefix),
		wleputils::FileUtil::writeToFile<uChar>(file_, this->header.version),
		wleputils::FileUtil::writeToFile<uChar>(file_, this->header.thumbnail_size_arr),
		wleputils::FileUtil::writeToFile<uChar>(file_, this->header.thumbnail_data)
	};

	for (int i = 0; i < items_to_write; i++) {
		if (bytes_written_items[i] < 0) {
			throw std::exception("Error while writing the WinLep header");
		}
		bytes_written += bytes_written_items[i];
	}

	return bytes_written;
}

std::string wlep::WLepWriter::debug_str() {
	std::string res = "";
	res.append("\nWritten header prefix: ")
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.header_prefix))
		.append("\nWritten version: ")
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.version))
		.append("\nWritten thumbnail size: ")
		.append(std::to_string(this->header.thumbnail_size))
		.append(" B\t-> 0x")
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.thumbnail_size_arr));

	return res;
}

void wlep::WLepWriter::openFileStream(std::string &filename) {
	if (!this->file_.is_open()) {
		this->file_.open(filename, std::ios::in | std::ios::binary);
	}
}

void wlep::WLepWriter::closeFileStream() {
	if (this->file_.is_open()) {
		this->file_.close();
	}
}
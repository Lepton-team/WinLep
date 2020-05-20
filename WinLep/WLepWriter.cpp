#include "WLepWriter.h"

WLep::WLepWriter::WLepWriter(std::string &filename, WLep::WLepHeader &header)
	:header(header) {
	if (filename.empty()) {
		throw std::invalid_argument("Filename cannot be empty!");
	}

	if (WLepUtils::StringUtil::ends_with(filename, WLepConstants::file_extension)) {
		filename.append(WLepConstants::file_extension);
	}

	open_fstream(filename);

	this->filename = filename;
}

WLep::WLepWriter::WLepWriter(std::string &filename, const std::string &thumbnail_filename) {
	if (filename.empty() || thumbnail_filename.empty()) {
		throw std::invalid_argument("Filename cannot be empty!");
	}

	this->header = WLep::WLepHeader(thumbnail_filename);

	if (!WLepUtils::StringUtil::ends_with(filename, WLepConstants::file_extension)) {
		filename.append(WLepConstants::file_extension);
	}

	open_fstream(filename);

	this->filename = filename;
}

WLep::WLepWriter::~WLepWriter() {
	close_fstream();
}

size_t WLep::WLepWriter::write_header() {
	size_t bytes_written = 0;
	const int items_to_write = 4;

	size_t bytes_written_items[items_to_write] = {
		WLepUtils::FileUtil::write_to_file<uChar>(file, this->header.header_prefix),
		WLepUtils::FileUtil::write_to_file<uChar>(file, this->header.version),
		WLepUtils::FileUtil::write_to_file<uChar>(file, this->header.thumbnail_size_arr),
		WLepUtils::FileUtil::write_to_file<uChar>(file, this->header.thumbnail_data)
	};

	for (int i = 0; i < items_to_write; i++) {
		if (bytes_written_items[i] < 0) {
			throw std::exception("Error while writing the WinLep header");
		}
		bytes_written += bytes_written_items[i];
	}

	return bytes_written;
}

std::string WLep::WLepWriter::debug_str() {
	std::string res = "";
	res.append("\nWritten header prefix: ")
		.append(WLepUtils::VectorUtil::hex_vec_to_hex_str(this->header.header_prefix))
		.append("\nWritten version: ")
		.append(WLepUtils::VectorUtil::hex_vec_to_hex_str(this->header.version))
		.append("\nWritten thumbnail size: ")
		.append(std::to_string(this->header.thumbnail_size))
		.append(" B\t-> 0x")
		.append(WLepUtils::VectorUtil::hex_vec_to_hex_str(this->header.thumbnail_size_arr));

	return res;
}

void WLep::WLepWriter::open_fstream(std::string &filename) {
	if (!this->filename.empty() && !this->file.is_open()) {
		file.open(filename, std::ios::in | std::ios::binary);
	}
}

void WLep::WLepWriter::close_fstream() {
	if (this->file.is_open()) {
		this->file.close();
	}
}
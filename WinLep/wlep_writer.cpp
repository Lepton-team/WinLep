#include "wlep_writer.h"
#include "file_util.h"
#include "string_util.h"
#include "wlep_constants.h"
#include "exception_util.h"
#include "vector_util.h"

//#define DEBUG

wlep::WLepWriter::WLepWriter(std::string &filename, wlep::WLepHeader &header)
	: header(header) {
	if (filename.empty()) {
		wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Filename cannot be empty!");
	}

	if (!wleputils::StringUtil::endsWith(filename, wlepconstants::file_extension)) {
		filename.append(wlepconstants::file_extension);
	}

	this->filename_ = filename;

	wleputils::FileUtil::openFileStream(this->file_, this->filename_, std::ios::out);
}

wlep::WLepWriter::WLepWriter(std::string &filename, IStream *thumbnail_data) {
	if (filename.empty()) {
		wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Filename cannot be empty!");
	}

	if (!wleputils::StringUtil::endsWith(filename, wlepconstants::file_extension)) {
		filename.append(wlepconstants::file_extension);
	}

	this->filename_ = filename;
	this->header = wlep::WLepHeader(thumbnail_data);

	wleputils::FileUtil::openFileStream(this->file_, this->filename_, std::ios::out);
}

wlep::WLepWriter::~WLepWriter() {
	wleputils::FileUtil::closeFileStream(this->file_);
}

size_t wlep::WLepWriter::writeHeader() {
	size_t bytes_written = 0;
	const int items_to_write = 4;

	size_t bytes_written_items[items_to_write] = {
		wleputils::FileUtil::writeToFile<uChar>(this->file_, this->header.header_prefix),
		wleputils::FileUtil::writeToFile<uChar>(this->file_, this->header.version),
		wleputils::FileUtil::writeToFile<uChar>(this->file_, this->header.thumbnail_size_arr),
		wleputils::FileUtil::writeToFile<uChar>(this->file_, this->header.thumbnail_data)
	};
	
	for (int i = 0; i < items_to_write; i++) {
		if (bytes_written_items[i] <= 0) {
			wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Error while writing the WinLep header");
		}
		bytes_written += bytes_written_items[i];
	}

	wleputils::FileUtil::closeFileStream(this->file_);

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
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.thumbnail_size_arr))
#ifdef DEBUG
		.append("\nWritten thumbnail data: ")
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.thumbnail_data));
#else
		;
#endif // DEBUG


	return res;
}

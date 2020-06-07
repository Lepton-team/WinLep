#define DEBUG
//#define PRINT_THUMBNAIL

#include "wlep_writer.h"
#include "file_util.h"
#include "wlep_constants.h"
#include "exception_util.h"
#include "vector_util.h"
#include "wlep_image.h"
#include "lepton_converter.h"

#ifdef TIME
#include <ctime>
#endif // TIME


wlep::WLepWriter::WLepWriter(std::string &wlep_filename, wlep::WLepHeader &header)
	: header(header) {
	if (wlep_filename.empty()) {
		wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Filename cannot be empty!");
	}

	this->wlep_filename_ = wlep_filename;
}

wlep::WLepWriter::WLepWriter(std::string &wlep_filename, IStream *thumbnail_data) {
	if (wlep_filename.empty()) {
		wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Filename cannot be empty!");
	}

	this->wlep_filename_ = wlep_filename;
	this->header = wlep::WLepHeader(thumbnail_data);
}

wlep::WLepWriter::WLepWriter(std::string &wlep_filename, std::string &jpg_filename) {
	if (wlep_filename.empty() || jpg_filename.empty()) {
		wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Filename cannot be empty!");
	}

	this->wlep_filename_ = wlep_filename;
	this->jpg_filename_ = jpg_filename;

	wlep::WLepImage *image = new wlep::WLepImage(jpg_filename);
	IStream *thumbnail = nullptr;

	image->createThumbnail(128);
	thumbnail = image->getThumbnailAsStream();

	this->header = wlep::WLepHeader(thumbnail);

#ifdef SAVE_THUMBNAIL
	Gdiplus::Image *img_from_stream = Gdiplus::Image::FromStream(thumbnail);
	wleputils::ImageUtil::save(L"img_from_stream_to_write.jpg", img_from_stream);
#endif // SAVE_THUMBNAIL

	thumbnail->Release();
	delete image;
}

wlep::WLepWriter::~WLepWriter() {
	wleputils::FileUtil::closeFileStream(this->wlep_file_);
}

#ifdef TIME
double diffClock(clock_t clock1, clock_t clock2) {
	double diffticks = clock1 - (double)clock2;
	double diffms = (diffticks) / (CLOCKS_PER_SEC / 1000);
	return diffms;
}
#endif // TIME

size_t wlep::WLepWriter::writeHeader() {
	size_t bytes_written = 0;
	const int items_to_write = 4;

	size_t bytes_written_items[items_to_write] = {
		wleputils::FileUtil::writeToFile<uChar>(this->wlep_file_, this->header.header_prefix),
		wleputils::FileUtil::writeToFile<uChar>(this->wlep_file_, this->header.version),
		wleputils::FileUtil::writeToFile<uChar>(this->wlep_file_, this->header.thumbnail_size_arr),
		wleputils::FileUtil::writeToFile<uChar>(this->wlep_file_, this->header.thumbnail_data)
	};

	for (int i = 0; i < items_to_write; i++) {
		if (bytes_written_items[i] <= 0) {
			wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Error while writing the WinLep header");
		}
		bytes_written += bytes_written_items[i];
	}

	return bytes_written;
}

size_t wlep::WLepWriter::writeLeptonData() {
	wlep::LeptonConverter converter;
#ifdef TIME
	clock_t start = std::clock();
#endif // TIME

	std::vector<uChar> lepton_data = converter.convertJpgToLepton(this->jpg_filename_);

#ifdef TIME
	clock_t end = std::clock();
	std::cerr << "[TIME] Converting to lepton took " << diffClock(end, start) << "ms\n";
#endif // TIME

	return wleputils::FileUtil::writeToFile<uChar>(this->wlep_file_, lepton_data);
}

size_t wlep::WLepWriter::writeWinLepFile() {
	wleputils::FileUtil::openFileStream(this->wlep_file_, this->wlep_filename_, std::ios::out);

#ifdef TIME
	clock_t start = std::clock();
#endif // TIME

	size_t header_written = writeHeader();

#ifdef TIME
	clock_t end = std::clock();
	std::cerr << "[TIME] Writing header took " << diffClock(end, start) << "ms\n";
#endif // TIME

	size_t lepton_data_written = writeLeptonData();
	wleputils::FileUtil::closeFileStream(this->wlep_file_);

	return header_written + lepton_data_written;
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
#ifdef PRINT_THUMBNAIL
		.append("\nWritten thumbnail data: ")
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.thumbnail_data));
#else
		;
#endif // PRINT_THUMBNAIL

	return res;
}

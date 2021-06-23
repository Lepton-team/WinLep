#define DEBUG
//#define TIME
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

wlep::WLepWriter::WLepWriter(const std::string &wlep_filename,
			     const std::string &jpg_filename,
			     const std::wstring &lepton_exe,
			     const std::wstring &lepton_args,
			     bool create_thumbnail) {
	if (wlep_filename.empty() || jpg_filename.empty()) {
		wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Filename cannot be empty!");
	}

	if (lepton_exe.empty()) {
		wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Lepton executable cannot be empty!");
	}

	if (lepton_args.empty()) {
		wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Lepton arguments cannot be empty!");
	}

	this->lepton_exe_ = lepton_exe;
	this->lepton_args_ = lepton_args;

	this->wlep_filename_ = wlep_filename;
	this->jpg_filename_ = jpg_filename;

	if (create_thumbnail) {
		wlep::WLepImage *image = new wlep::WLepImage(jpg_filename);
		IStream *thumbnail = nullptr;
		image->createThumbnail(wlepconstants::thumbnail_side_length);
		thumbnail = image->getThumbnailAsStream();

		this->header = wlep::WLepHeader(thumbnail);

#ifdef SAVE_THUMBNAIL
		Gdiplus::Image *img_from_stream = Gdiplus::Image::FromStream(thumbnail);
		wleputils::ImageUtil::save(L"img_from_stream_to_write.jpg", img_from_stream);
#endif // SAVE_THUMBNAIL

		thumbnail->Release();
		delete image;
	} else {
		this->header = wlep::WLepHeader();
	}

}

wlep::WLepWriter::~WLepWriter() {
	wleputils::FileUtil::closeFileStream(this->wlep_file_);
	wleputils::FileUtil::closeFileStream(this->jpg_file_);
}

#ifdef TIME
double clockDiff(clock_t clock1, clock_t clock2) {
	double diffticks = clock1 - (double)clock2;
	double diffms = (diffticks) / (CLOCKS_PER_SEC / 1000);
	return diffms;
}
#endif // TIME

size_t wlep::WLepWriter::writeHeader() {
	size_t bytes_written = 0;
	constexpr int items_to_write = 4;

	size_t bytes_written_items[items_to_write] = {
		wleputils::FileUtil::writeToFile<uChar>(this->wlep_file_, this->header.header_prefix),
		wleputils::FileUtil::writeToFile<uChar>(this->wlep_file_, this->header.version),
		wleputils::FileUtil::writeToFile<uChar>(this->wlep_file_, this->header.thumbnail_size_arr),
		wleputils::FileUtil::writeToFile<uChar>(this->wlep_file_, this->header.thumbnail_data)
	};

	for (int i = 0; i < items_to_write; i++) {
		if (bytes_written_items[i] <= 0) {
			wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Error while writing WinLep header");
		}
		bytes_written += bytes_written_items[i];
	}

	return bytes_written;
}

size_t wlep::WLepWriter::writeWinLepFile() {
	wleputils::FileUtil::openFileStream(this->wlep_file_, this->wlep_filename_, std::ios::out);

#ifdef TIME
	clock_t start = std::clock();
#endif // TIME

	wlep::LeptonConverter converter(this->lepton_exe_, this->lepton_args_);

#ifdef TIME
	clock_t start = std::clock();
#endif // TIME
	std::wstring wide_filename = wleputils::StringUtil::toWideString(this->jpg_filename_);
	std::vector<uChar> lepton_data = std::vector<uChar>(); 
	
	try {
		lepton_data = converter.convertJpgToLepton(wide_filename);
	} catch (std::exception ex) {
		wleputils::ExceptionUtil::printErrorMsg("Error while converting Lepton to jpg - " + std::string(ex.what()));
	} catch (...) {
		wleputils::ExceptionUtil::printErrorMsg("Error while converting Lepton to jpg - Other exception");
	}
	
#ifdef TIME
	clock_t end = std::clock();
	std::cerr << "[TIME] Converting to lepton took " << clockDiff(end, start) << "ms\n";
#endif // TIME

#ifdef TIME
	start = std::clock();
#endif // TIME

	size_t bytes_written = -1;

	if (!lepton_data.empty()) {
		bytes_written = writeHeader();
		bytes_written += wleputils::FileUtil::writeToFile<uChar>(this->wlep_file_, lepton_data);
	}

	if (bytes_written == -1) {
		wleputils::ExceptionUtil::printErrorMsg("No bytes written to file '" + this->wlep_filename_ + "' !");
	}

#ifdef TIME
	end = std::clock();
	std::cerr << "[TIME] Writing to file took " << clockDiff(end, start) << "ms\n";
#endif // TIME

#ifdef TIME
	clock_t end = std::clock();
	std::cerr << "[TIME] Writing header took " << clockDiff(end, start) << "ms\n";
#endif // TIME

	wleputils::FileUtil::closeFileStream(this->wlep_file_);

	return bytes_written;
}

size_t wlep::WLepWriter::writeJpgFile(std::vector<uChar> &lepton_data, bool clear_lepton_data) {
	wlep::LeptonConverter converter(this->lepton_exe_, this->lepton_args_);
	std::vector<uChar> jpg_data = std::vector<uChar>();

	try {
		jpg_data = converter.convertLeptonToJpg(lepton_data);
	} catch (std::exception ex) {
		wleputils::ExceptionUtil::printErrorMsg("Error while converting Lepton to jpg - " + std::string(ex.what()));
	} catch (...) {
		wleputils::ExceptionUtil::printErrorMsg("Error while converting Lepton to jpg - Other exception");
	}

	if (clear_lepton_data) {
		lepton_data.clear();
		lepton_data = std::vector<uChar>();
	}

	size_t bytes_written = -1;

	if (!jpg_data.empty()) {
		wleputils::FileUtil::openFileStream(this->jpg_file_, this->jpg_filename_, std::ios::out);
		bytes_written = wleputils::FileUtil::writeToFile<uChar>(this->jpg_file_, jpg_data);
		wleputils::FileUtil::closeFileStream(this->jpg_file_);
	}

	if (bytes_written == -1) {
		wleputils::ExceptionUtil::printErrorMsg("No bytes written to file '" + this->jpg_filename_ + "' !");
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
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.thumbnail_size_arr))
#ifdef PRINT_THUMBNAIL
		.append("\nWritten thumbnail data: ")
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.thumbnail_data));
#else
		;
#endif // PRINT_THUMBNAIL

	return res;
}

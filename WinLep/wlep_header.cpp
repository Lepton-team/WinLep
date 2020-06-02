#include "wlep_header.h"
#include "vector_util.h"
#include "exception_util.h"
#include <stdio.h>

std::streampos wlep::WLepHeader::calculateFileSize(std::ifstream &file) {
	file.seekg(0, file.beg);

	std::streampos size = file.tellg();
	file.seekg(0, file.end);
	size = file.tellg() - size;

	file.seekg(0, file.beg);

	return size;
}

// FIXME: Temporary
std::vector<uChar> wlep::WLepHeader::createThumbnailData(std::ifstream &thumbnail) {
	std::vector<uChar> res;

	uChar ch = thumbnail.get();
	while (thumbnail.good()) {
		res.push_back(ch);
		ch = thumbnail.get();
	}

	return res;
}

HRESULT seekBackToBeginning(IStream *stream) {
	LARGE_INTEGER li;
	li.QuadPart = 0;
	return stream->Seek(li, STREAM_SEEK_SET, NULL);
}

void wlep::WLepHeader::createThumbnailData(IStream *thumbnail_data_stream, ULONGLONG stream_size) {
	uChar *stream_data = new uChar[stream_size];
	ULONG bytes_saved = 0;
	// Seek back to the beginning of the stream
	seekBackToBeginning(thumbnail_data_stream);

	HRESULT hr = thumbnail_data_stream->Read(stream_data, stream_size, &bytes_saved);

	if (FAILED(hr)) {
		wleputils::ExceptionUtil::throwAndPrintException<std::exception>("Error while reading from thumbnail IStream!");
	} else {
		// Create thumbnail data
		this->thumbnail_data = std::vector<uChar>();
		this->thumbnail_data.reserve(stream_size);

		wleputils::VectorUtil::arrayToVector(this->thumbnail_data, stream_data, stream_size);

		seekBackToBeginning(thumbnail_data_stream);
		delete[] stream_data;
	}
}

void wlep::WLepHeader::createThumbnailSize(size_t size) {
	this->thumbnail_size = size;

	uChar *buf = new uChar[wlepconstants::thumbnail_size_size];
	memcpy(buf, (uChar *)&size, sizeof(uChar) * wlepconstants::thumbnail_size_size);

	wleputils::VectorUtil::arrayToVector(this->thumbnail_size_arr, buf, wlepconstants::thumbnail_size_size);
	delete[] buf;

	if (!isBigEndian()) {
		std::reverse(std::begin(this->thumbnail_size_arr), std::end(this->thumbnail_size_arr));
	}
}

bool wlep::WLepHeader::isBigEndian() {
	union {
		uint32_t i;
		char c[4];
	} onion = {0x01020304};

	return onion.c[0] == 1;
}

wlep::WLepHeader::WLepHeader(IStream *thumbnail_data_stream) {
	if (!thumbnail_data_stream) {
		wleputils::ExceptionUtil::throwAndPrintException<std::invalid_argument>("Thumbnail data stream cannot be empty!");
	}

	this->data = std::vector<uChar>();
	this->thumbnail_size_arr = std::vector<uChar>();
	this->exif_size_arr = std::vector<uChar>();
	this->header_prefix = std::vector<uChar>();
	this->version = std::vector<uChar>();

	wleputils::VectorUtil::append(wlepconstants::header_prefix, this->header_prefix);
	wleputils::VectorUtil::append(wlepconstants::version, this->version);

	STATSTG stream_stats = {0};
	HRESULT stat_hr = thumbnail_data_stream->Stat(&stream_stats, 0);

	if (FAILED(stat_hr)) {
		wleputils::ExceptionUtil::throwAndPrintException<std::exception>("Error while reading the thumbnail IStream stats!");
	} else {
		createThumbnailSize(stream_stats.cbSize.QuadPart);
		createThumbnailData(thumbnail_data_stream, stream_stats.cbSize.QuadPart);
	}

}

wlep::WLepHeader::WLepHeader() {
	thumbnail_size_arr = std::vector<uChar>();
	exif_size_arr = std::vector<uChar>();
	thumbnail_data = std::vector<uChar>();
	exif_data = std::vector<uChar>();
	data = std::vector<uChar>();
	header_prefix = std::vector<uChar>();
	version = std::vector<uChar>();

	exif_size = 0;
	thumbnail_size = 0;
}
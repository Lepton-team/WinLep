#include "WLepHeader.h"

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

bool wlep::WLepHeader::isBigEndian() {
	union {
		uint32_t i;
		char c[4];
	} onion = {0x01020304};

	return onion.c[0] == 1;
}

wlep::WLepHeader::WLepHeader(std::string const &thumbnail_filename) {
	if (thumbnail_filename.empty()) {
		throw std::invalid_argument("Thumbnail filename cannot be empty!");
	}

	this->data = std::vector<uChar>();
	this->thumbnail_size_arr = std::vector<uChar>();
	this->exif_size_arr = std::vector<uChar>();
	this->header_prefix = std::vector<uChar>();
	this->version = std::vector<uChar>();

	wleputils::VectorUtil::append(wlepconstants::header_prefix, this->header_prefix);
	wleputils::VectorUtil::append(wlepconstants::version, this->version);

	//TODO: Resize thumbnail
	std::ifstream thumbnail;
	thumbnail.open(thumbnail_filename, std::ios::binary | std::ios::in);

	size_t f_size = calculateFileSize(thumbnail);

	uChar *buf = new uChar[wlepconstants::thumbnail_size_size];
	memcpy(buf, (uChar *)&f_size, sizeof(uChar) * wlepconstants::thumbnail_size_size);

	wleputils::VectorUtil::arrayToVector(this->thumbnail_size_arr, buf, wlepconstants::thumbnail_size_size);

	delete[] buf;

	if (!isBigEndian()) {
		std::reverse(std::begin(this->thumbnail_size_arr), std::end(this->thumbnail_size_arr));
	}

	this->thumbnail_data = createThumbnailData(thumbnail);

	thumbnail.close();

	//VectorUtil::append<std::vector<uChar>, uChar>(thumbnail_data, this->data);
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
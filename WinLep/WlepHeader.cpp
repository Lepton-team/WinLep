#include "WLepHeader.h"

std::streampos WLep::WLepHeader::file_size(std::ifstream &file) {
	file.seekg(0, file.beg);

	std::streampos size = file.tellg();
	file.seekg(0, file.end);
	size = file.tellg() - size;

	file.seekg(0, file.beg);

	return size;
}

// FIXME: Temporary
std::vector<uChar> WLep::WLepHeader::create_thumbnail_data(std::ifstream &thumbnail) {
	std::vector<uChar> res;

	uChar ch = thumbnail.get();
	while (thumbnail.good()) {
		res.push_back(ch);
		ch = thumbnail.get();
	}

	return res;
}

bool WLep::WLepHeader::is_big_endian() {
	union {
		uint32_t i;
		char c[4];
	} onion = {0x01020304};

	return onion.c[0] == 1;
}

WLep::WLepHeader::WLepHeader(std::string const &thumbnail_filename) {
	if (thumbnail_filename.empty()) {
		throw std::invalid_argument("Thumbnail filename cannot be empty!");
	}

	this->data = std::vector<uChar>();
	this->thumbnail_size_arr = std::vector<uChar>();
	this->exif_size_arr = std::vector<uChar>();
	this->header_prefix = std::vector<uChar>();
	this->version = std::vector<uChar>();

	WLepUtils::VectorUtil::append(WLepConstants::header_prefix, this->header_prefix);
	WLepUtils::VectorUtil::append(WLepConstants::version, this->version);

	//TODO: Resize thumbnail
	std::ifstream thumbnail;
	thumbnail.open(thumbnail_filename, std::ios::binary | std::ios::in);

	size_t f_size = file_size(thumbnail);

	uChar *buf = new uChar[WLepConstants::thumbnail_size_size];
	memcpy(buf, (uChar *)&f_size, sizeof(uChar) * WLepConstants::thumbnail_size_size);
	WLepUtils::VectorUtil::to_vector(this->thumbnail_size_arr, buf, WLepConstants::thumbnail_size_size);

	delete[] buf;

	if (!is_big_endian()) {
		std::reverse(std::begin(this->thumbnail_size_arr), std::end(this->thumbnail_size_arr));
	}

	this->thumbnail_data = create_thumbnail_data(thumbnail);

	thumbnail.close();

	//VectorUtil::append<std::vector<uChar>, uChar>(thumbnail_data, this->data);
}

WLep::WLepHeader::WLepHeader() {
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
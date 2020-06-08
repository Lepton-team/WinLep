#include "wlep_reader.h"
#include "vector_util.h"
#include "file_util.h"
#include "exception_util.h"

//#define PRINT_THUMBNAIL

void wlep::WLepReader::readAndValidateHeader(bool read_thumbnail) {
	wleputils::FileUtil::openFileStream(this->file_, this->filename_, std::ios::in);
	// Header prefix
	readFromFileToVector(this->header.header_prefix, wlepconstants::header_prefix_size);

	for (int i = 0; i < wlepconstants::header_prefix_size; i++) {
		if (this->header.header_prefix[i] != wlepconstants::header_prefix[i]) {
			std::string msg = this->filename_ + " isn't a wlep file!";
			wleputils::ExceptionUtil::throwAndPrintException
				<std::exception>("Invalid file format!", msg.c_str());
		}
	}

	// Version
	readFromFileToVector(this->header.version, wlepconstants::version_size);

	// Thumbnail size
	readFromFileToVector(this->header.thumbnail_size_arr, wlepconstants::thumbnail_size_size);
	this->header.thumbnail_size = wleputils::VectorUtil::hexVectorToInt(this->header.thumbnail_size_arr);

	if (read_thumbnail) {
		// Thumbnail data
		readFromFileToVector(this->header.thumbnail_data, this->header.thumbnail_size);
	} else {
		// Just move over the thumbnail data
		std::streampos pos = this->file_.tellg() + std::streampos(this->header.thumbnail_size);
		this->file_.seekg(pos);
	}
}

std::vector<uChar> wlep::WLepReader::validateFileAndReadLeptonData() {
	wleputils::FileUtil::openFileStream(this->file_, this->filename_, std::ios::in);
	readAndValidateHeader(false);
	// The GET pointer is on the beginning of the lepton_data after reading the header

	// For some reason this is much faster than lepton_data(std::istreambuf_iterator<uChar>(this->file_), {});
	std::vector<uChar> lepton_data;
	constexpr unsigned int buff_size = 1024 * 4; // Size of one page
	uChar *buff = new uChar[buff_size];

	while (!this->file_.eof()) {
		this->file_.read(buff, buff_size);
		size_t data_read = this->file_.gcount();
		if (data_read == 0) {
			break;
		}
		lepton_data.insert(lepton_data.end(), buff, buff + data_read);
	}

	delete[] buff;

	wleputils::FileUtil::closeFileStream(this->file_);
	return lepton_data;
}

void wlep::WLepReader::readFromFileToVector(std::vector<uChar> &vec, size_t bytes) {
	if (!this->file_.good()) {
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
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.thumbnail_size_arr))
#ifdef PRINT_THUMBNAIL
		.append("\nRead thumbnail data: ")
		.append(wleputils::VectorUtil::hexVectorToHexString(this->header.thumbnail_data));
#else 
		;
#endif // PRINT_THUMBNAIL

	return res;
}

wlep::WLepReader::WLepReader(std::string const &filename) {
	if (filename.empty()) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::invalid_argument>("Filename cannot be empty!");
	}
	this->filename_ = filename;
	header = wlep::WLepHeader();
}

wlep::WLepReader::~WLepReader() {
	wleputils::FileUtil::closeFileStream(this->file_);
}
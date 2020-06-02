#include "wlep_image.h"
#include "file_util.h"
#include "exception_util.h"

wlep::WLepImage::WLepImage(const std::string &filename) {
	GdiplusStartup(&this->gdiplusToken_, &this->gdiplusStartupInput_, NULL);
	this->filename_ = std::wstring(filename.begin(), filename.end());

	this->image = new Gdiplus::Image(this->filename_.c_str());
}

wlep::WLepImage::~WLepImage() {
	if (mem_) {
		::GlobalFree(mem_);
	}

	delete thumbnail;
	delete image;

	Gdiplus::GdiplusShutdown(this->gdiplusToken_);
}

Gdiplus::Image *wlep::WLepImage::createThumbnail(UINT width, UINT height) {
	this->thumbnail = this->image->GetThumbnailImage(width, height);
	std::string filename = std::string(this->filename_.begin(), this->filename_.end());

#ifdef DEBUG
	CLSID jpgClsid;
	std::cout << "Image dimensions: " << this->image->GetWidth() << "px x " << this->image->GetHeight() << "px \n";
	std::cout << "Thumbnail dimensions: " << thumbnail->GetWidth() << "px x " << thumbnail->GetHeight() << "px \n";
	wleputils::ImageUtil::save(L"thumb.jpg", thumbnail);
#endif // DEBUG

	return this->thumbnail;
}

Gdiplus::Image *wlep::WLepImage::createThumbnail(UINT side_length) {
	UINT img_width = this->image->GetWidth();
	UINT img_height = this->image->GetHeight();
	UINT calculated_height = side_length;
	UINT calculated_width = side_length;

	// To keep the aspect ratio of the original image
	if (img_width > img_height) {
		calculated_height = (img_height * side_length) / img_width;
	} else if (img_width < img_height) {
		calculated_width = (img_width * side_length) / img_height;
	}

	return this->createThumbnail(calculated_width, calculated_height);
}

Gdiplus::Bitmap *wlep::WLepImage::getThumbnailAsBitmap() {
	if (this->thumbnail == nullptr) {
		return nullptr;
	}
	// Try to convert the thumbnail using dynamic cast
	Gdiplus::Bitmap *thumbnail_bmp = dynamic_cast<Gdiplus::Bitmap *>(this->thumbnail);

	// The thumbnail is an Image which is not a Bitmap. Convert.
	if (!thumbnail_bmp) {
		thumbnail_bmp = wleputils::ImageUtil::gdiplusImageToBitmap(this->thumbnail);
	}

#ifdef DEBUG
	if (thumbnail_bmp) {
		std::cout << "Thumbnail bitmap dimensions: " << thumbnail_bmp->GetWidth() << "px x " << thumbnail_bmp->GetHeight() << "px\n";
	}
#endif // DEBUG

	// The thumbnail is a Bitmap so just deal with it.
	return thumbnail_bmp;
}

IStream *wlep::WLepImage::getThumbnailAsStream() throw(std::exception) {
	if (this->thumbnail == nullptr) {
		return nullptr;
	}

	mem_ = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(this->thumbnail));
	if (!mem_) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::exception>("Error while allocationg memory!");
	}

	IStream *thumbnail_stream;
	HRESULT hr = ::CreateStreamOnHGlobal(mem_, true, &thumbnail_stream);

	if (hr != S_OK) {
		wleputils::ExceptionUtil::throwAndPrintException
			<std::exception>("Error while creating IStream!");
	}

	try {
		wleputils::ImageUtil::save(thumbnail_stream, this->thumbnail);
	} catch (...) {
		wleputils::ExceptionUtil::printErrorMsg("Error while saving Image to IStream!");
	}

	return thumbnail_stream;
}

HRESULT seekToBeginning(IStream *stream) {
	LARGE_INTEGER li;
	li.QuadPart = 0;
	return stream->Seek(li, STREAM_SEEK_SET, NULL);
}

BYTE *wlep::WLepImage::getThumbnailAsRawData() {
	IStream *stream = nullptr;

	try {
		stream = getThumbnailAsStream();
	} catch (...) {
		wleputils::ExceptionUtil::printErrorMsg("Error while getting thumbnail as Stream!");
		return nullptr;
	}

	STATSTG stats = {0};
	HRESULT stat_hr = stream->Stat(&stats, 0);
	uChar *stream_data = new uChar[stats.cbSize.QuadPart];
	ULONG bytes_saved = 0;

	// Seek back to the beginning of the stream
	if (FAILED(seekToBeginning(stream))) {
		wleputils::ExceptionUtil::throwAndPrintException<std::exception>("Error when seeking to beginning of the IStream!");
	}
	HRESULT hr = stream->Read(stream_data, stats.cbSize.QuadPart, &bytes_saved);

#ifdef DEBUG
	std::cout << "Read " << bytes_saved << " B of thumbnail's raw data\n";
#endif // DEBUG

	if (bytes_saved == 0) {
		wleputils::ExceptionUtil::printErrorMsg("Read 0 bytes from IStream!");
	}

	if (FAILED(hr)) {
		wleputils::ExceptionUtil::throwAndPrintException<std::exception>("Error while reading from thumbnail IStream!");
	} else {
		return stream_data;
	}



	delete[] stream_data;

	return nullptr;
}

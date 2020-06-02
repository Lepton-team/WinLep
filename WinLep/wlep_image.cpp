#include "wlep_image.h"
#include "file_util.h"

wlep::WLepImage::WLepImage(const std::string &filename) {
	GdiplusStartup(&this->gdiplusToken_, &this->gdiplusStartupInput_, NULL);
	this->filename_ = std::wstring(filename.begin(), filename.end());

	this->image = new Gdiplus::Image(this->filename_.c_str());
}

wlep::WLepImage::~WLepImage() {
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
	this->thumbnail_bmp = dynamic_cast<Gdiplus::Bitmap *>(this->thumbnail);

	// The thumbnail is an Image which is not a Bitmap. Convert.
	if (!this->thumbnail_bmp) {
		this->thumbnail_bmp = wleputils::ImageUtil::gdiplusImageToBitmap(this->thumbnail);
	}

#ifdef DEBUG
	if (thumbnail_bmp) {
		std::cout << "Thumbnail bitmap dimensions: " << this->thumbnail_bmp->GetWidth() << "px x " << this->thumbnail_bmp->GetHeight() << "px\n";
	}
#endif // DEBUG


	// The thumbnail is a Bitmap so just deal with it.
	return this->thumbnail_bmp;
}

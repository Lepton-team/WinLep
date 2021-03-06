#pragma once
#define DEBUG
#include <string>
#include "image_util.h"

#ifdef DEBUG
#include <iostream>
#endif // DEBUG

namespace wlep {
	class WLepImage {
	private:
		Gdiplus::GdiplusStartupInput gdiplusStartupInput_;
		ULONG_PTR gdiplusToken_;
		std::wstring filename_;
		HGLOBAL mem_;

	public:
		Gdiplus::Image *image;
		Gdiplus::Image *thumbnail;

		WLepImage(const std::string &filename);
		WLepImage(std::vector<BYTE> &jpg_data, const std::string &filename);
		~WLepImage();

		void save();

		// Doesn't preserve aspect ratio
		Gdiplus::Image *createThumbnail(UINT width, UINT height);
		// Preserves aspect ratio
		Gdiplus::Image *createThumbnail(UINT side_length);

		Gdiplus::Bitmap *getThumbnailAsBitmap();
		IStream *getThumbnailAsStream();
		BYTE *getThumbnailAsRawData();
	};
}


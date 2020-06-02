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
		Gdiplus::Bitmap *thumbnail_bmp;
		IStream *thumbnail_stream;

		WLepImage(const std::string &filename);
		~WLepImage();

		// Doesn't preserve aspect ratio
		Gdiplus::Image *createThumbnail(UINT width, UINT height);
		// Preserves aspect ratio
		Gdiplus::Image *createThumbnail(UINT side_length);

		Gdiplus::Bitmap *getThumbnailAsBitmap();
		IStream *getThumbnailAsStream();
	};
}


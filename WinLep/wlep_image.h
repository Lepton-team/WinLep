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
		Gdiplus::Image *createThumbnail(UINT width, UINT height);

	public:
		Gdiplus::Image *image;
		WLepImage(const std::string &filename);
		~WLepImage();

		Gdiplus::Image *createThumbnail(UINT side_length);
	};
}


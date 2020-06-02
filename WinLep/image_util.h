#pragma once
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <string>
#include <iostream>
#include "wlep_constants.h"

namespace wleputils {
	class ImageUtil {
	public:
		
		static inline std::wstring getImageType(std::string &file_extension) {
			return wlepconstants::sup_file_extension_img_format_map[file_extension];
		}

		//TODO:
		static inline bool save(const std::wstring &filename, Gdiplus::Image *img) {
			//wleputils::ImageUtil::getEncoderClsid(L"image/jpeg", &jpgClsid);
			//img->Save(filename.c_str(), &jpgClsid);
			return false;
		}

		/*
			Sets the CLSID to the corresponding encoder for given format		
		*/
		static int getEncoderClsid(const WCHAR *format, CLSID *pClsid) {
			UINT num = 0;          // number of image encoders
			UINT size = 0;         // size of the image encoder array in bytes

			Gdiplus::ImageCodecInfo *pImageCodecInfo = NULL;

			Gdiplus::GetImageEncodersSize(&num, &size);
			if (size == 0) {
				return -1;  // Failure
			}

			pImageCodecInfo = (Gdiplus::ImageCodecInfo *)(malloc(size));
			if (pImageCodecInfo == NULL) {
				return -1;  // Failure
			}

			GetImageEncoders(num, size, pImageCodecInfo);

			for (UINT j = 0; j < num; ++j) {
				if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
					*pClsid = pImageCodecInfo[j].Clsid;
					free(pImageCodecInfo);
					return j;  // Success
				}
			}
			free(pImageCodecInfo);

			return -1;  // Failure
		}
	};
}
#pragma once
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <string>
#include "wlep_constants.h"
#include "exception_util.h"

namespace wleputils {
	class ImageUtil {
	public:

		static inline std::wstring getImageFormat(std::string &file_extension) {
			return wlepconstants::sup_file_extension_img_format_map[file_extension];
		}

		static inline std::string getFileExtension(std::string &filename) {
			std::string::size_type idx;
			idx = filename.rfind('.');

			if (idx != std::string::npos) {
				return filename.substr(idx + 1);
			}

			return nullptr;
		}

		static void save(const std::wstring &filename, Gdiplus::Image *img) {
			CLSID jpgClsid;
			std::string str_filename = std::string(filename.begin(), filename.end());
			std::string extension = getFileExtension(str_filename);
			std::wstring format = getImageFormat(extension);

			if (format.empty()) {
				std::string msg = extension + " is not a supported file extension!";
				wleputils::ExceptionUtil::throwAndPrintException<std::exception>(msg);
			}

			getEncoderClsid(format.c_str(), &jpgClsid);

			// Save the image
			if (img->Save(filename.c_str(), &jpgClsid) != Gdiplus::Status::Ok) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while saving image!");
			}
		}

		/*
			Sets the CLSID to the corresponding encoder for a given format
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
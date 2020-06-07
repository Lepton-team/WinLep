#pragma once
#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <string>
#include "wlep_constants.h"
#include "exception_util.h"
#include "string_util.h"

namespace wleputils {
	class ImageUtil {
	public:

		/*
			Returns a image format based on a file extension (e.g. "image/jpeg")
			If the given file extension isn't supported returns an empty string
		*/
		static inline std::wstring getImageFormat(std::string file_extension) {
			try {
				wleputils::StringUtil::toLowerCase(file_extension);
				return wlepconstants::sup_file_extension_img_format_map.at(file_extension);
			} catch (...) {
				return L"";
			}
		}

		static inline std::wstring getImageFormat(const char *file_extension) {
			std::string str_extension = std::string(file_extension);
			return getImageFormat(str_extension);
		}

		static inline std::string getFileExtension(std::string &filename) {
			std::string::size_type idx;
			idx = filename.rfind('.');

			if (idx != std::string::npos) {
				return filename.substr(idx + 1);
			}

			return nullptr;
		}

		/*
			Creates a Bitmap and paints the given image into it.
		*/
		static Gdiplus::Bitmap *gdiplusImageToBitmap(Gdiplus::Image *img, Gdiplus::Color background = Gdiplus::Color::Transparent) {
			Gdiplus::Bitmap *bmp = nullptr;
			try {
				int width = img->GetWidth();
				int height = img->GetHeight();
				auto format = img->GetPixelFormat();

				bmp = new Gdiplus::Bitmap(width, height, format);
				auto gfx = std::unique_ptr<Gdiplus::Graphics>(Gdiplus::Graphics::FromImage(bmp));

				gfx->Clear(background);
				gfx->DrawImage(img, 0, 0, width, height);
			} catch (...) {
				// this might happen if img->GetPixelFormat() is something exotic
				// ... not sure
				wleputils::ExceptionUtil::printErrorMsg("Error while creating Bitmap from Image!");
				return nullptr;
			}
			return bmp;
		}

		/*
			Saves given image to the provided file name.
			Filename must contain a extension!
			Because based on it, it determines the image format
		*/
		static void save(const std::wstring &filename, Gdiplus::Image *img) {
			CLSID clsid;
			std::string str_filename = std::string(filename.begin(), filename.end());
			std::string extension = getFileExtension(str_filename);

			if (extension.empty()) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::invalid_argument>("Filename must contain an extension! (e.g. .jpg)");
			}

			std::wstring format = getImageFormat(extension);

			if (format.empty()) {
				std::string msg = extension + " is not a supported file extension!";
				wleputils::ExceptionUtil::throwAndPrintException<std::exception>(msg);
			}

			getEncoderClsid(format.c_str(), &clsid);

			// Save the image
			if (img->Save(filename.c_str(), &clsid) != Gdiplus::Status::Ok) {
				std::string msg = "Error while saving " + str_filename + " image to file!";
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>(msg);
			}
		}

		/*
			Saves given image to a given IStream
		*/
		static void save(IStream *img_stream, Gdiplus::Image *img) {
			CLSID clsid;
			std::wstring format = getImageFormat("jpg");
			getEncoderClsid(format.c_str(), &clsid);

			// Save the image
			if (img->Save(img_stream, &clsid) != Gdiplus::Status::Ok) {
				wleputils::ExceptionUtil::throwAndPrintException
					<std::exception>("Error while saving image to stream!");
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
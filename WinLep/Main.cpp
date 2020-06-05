#define DEBUG 
#define SAVE_THUMBNAIL

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <Windows.h>

#include "lepton_convertor.h"
#include "wlep_reader.h"
#include "wlep_writer.h"
#include "wlep_image.h"

std::string out_filename = "out.wlep";
std::string test_lep_filename = "test.wlep";
std::string test_jpg_filename = "test.jpg";

/*
	This code has been written according to Google's C++ style standards.
	If you were to contribute to this, please use this coding style.
	https://google.github.io/styleguide/cppguide.html

	Since C++11, you may want to use either snake_case or camelCase for function names.
	This is because to make a class work as the range-expression in a range-based for-loop,
	you have to define functions called begin and end (case-sensitive) for that class.
	https://stackoverflow.com/questions/1776291/function-names-in-c-capitalize-or-not

	Therefore for function names prefer camelCase instead of PascalCase.
*/
void testReading() {
	wlep::WLepReader reader(out_filename);
	reader.readHeader();

	std::cout << reader.debug_str() << '\n';
}

void testWriting() {
	wlep::WLepImage *image = new wlep::WLepImage(test_jpg_filename);
	try {
		image->createThumbnail(128);
		Gdiplus::Bitmap *bmp = image->getThumbnailAsBitmap();
		IStream *stream = image->getThumbnailAsStream();
		uChar *raw_data = image->getThumbnailAsRawData();

#ifdef SAVE_THUMBNAIL
		Gdiplus::Image *img_from_stream = Gdiplus::Image::FromStream(stream);
		wleputils::ImageUtil::save(L"img_from_stream.jpg", img_from_stream);
#endif // SAVE_THUMBNAIL

		wlep::WLepWriter writer(out_filename, stream);
		writer.writeHeader();
		std::cout << writer.debug_str() << '\n';

		delete bmp;
		delete raw_data;
		delete image;
	} catch (...) {

	}

}

int main(int argc, char **argv) {
	testWriting();
	testReading();

	wlep::LeptonConvertor lepton;
	std::vector<uChar> lepton_data = lepton.convertJpgToLepton(test_jpg_filename);

	std::cout << "Done.\n";
		 
	return 0;
}
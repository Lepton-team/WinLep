#include <windows.h>
#include <climits>

#include "string_util.h"
#include "wlep.h"
#include "wlep_lib.h"
#include "wlep_reader.h"
#include "lepton_converter.h"

std::vector<unsigned char> convertLeptonToJpg(const std::string wlep_filename) {
	wlep::LeptonConverter converter(g_lepton_exe, g_lepton_args);
	wlep::WLepReader reader(wlep_filename);
	std::vector<uChar> lepton_data = reader.validateFileAndReadLeptonData();
	std::vector<unsigned char> jpeg_data = converter.convertLeptonToJpg(lepton_data);
	return jpeg_data;
}

// Taken from https://github.com/lunnlew/weicai-scraper
// under MIT license
std::string Wchar_tToString(const wchar_t* wchar)
{
	std::string szDst;
	const wchar_t* wText = wchar;
	DWORD dwNum = WideCharToMultiByte(1250, NULL, wText, -1, NULL, 0, NULL, FALSE);
	char* psText;
	psText = new char[dwNum];
	WideCharToMultiByte(1250, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	szDst = psText;
	delete[]psText;
	return szDst;
}

LPVOID wlepToJpg(const std::wstring& wlep_filename, unsigned int *n_bytes) {
	wlep::LeptonConverter converter(g_lepton_exe, g_lepton_args);

	std::string std_filename = Wchar_tToString(wlep_filename.data());
	wlep::WLepReader reader(std_filename);
	std::vector<uChar> lepton_data = reader.validateFileAndReadLeptonData();
	std::vector<unsigned char> jpeg_data = converter.convertLeptonToJpg(lepton_data);

	size_t jpeg_size = jpeg_data.size();
	// Check for overflow because of the interface
	if (jpeg_size > UINT_MAX)
		return nullptr;

	unsigned char* buffer = (unsigned char*) HeapAlloc(GetProcessHeap(), 0,
							   jpeg_data.size());
	if (!buffer)
		return nullptr;

	std::copy(jpeg_data.begin(), jpeg_data.end(), buffer);
	*n_bytes = static_cast<unsigned int>(jpeg_size);

	return buffer;
}

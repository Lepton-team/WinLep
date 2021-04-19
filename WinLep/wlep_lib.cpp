#include "wlep.h"
#include "wlep_lib.h"
#include "wlep_reader.h"
#include "lepton_converter.h"

std::vector<unsigned char> convertLeptonToJpg(const std::string wlep_filename) {
	// TODO: This should be called just once, preferably in DllMain
	const std::wstring module_dir_path = getModuleDirectory();

	wlep::LeptonConverter converter(module_dir_path, g_lepton_exe, g_lepton_args);
	wlep::WLepReader reader(wlep_filename);
	std::vector<uChar> lepton_data = reader.validateFileAndReadLeptonData();
	std::vector<unsigned char> jpeg_data = converter.convertLeptonToJpg(lepton_data);
	return jpeg_data;
}
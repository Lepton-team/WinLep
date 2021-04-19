#pragma once
#include <vector>
#include <string>

#define DLL_EXPORT __declspec(dllexport)

const std::wstring getModuleDirectory();
DLL_EXPORT std::vector<unsigned char> convertLeptonToJpg(const std::string jpg_filename);
#pragma once
#include <vector>
#include <string>
#include <Windef.h>

#define DLL_EXPORT __declspec(dllexport)

DLL_EXPORT std::vector<unsigned char> convertLeptonToJpg(const std::string jpg_filename);

/**
 * Read @param wlep_filename and return byte array containing converted JPEG.
 *
 * Returned buffer has to be freed after use by the user:
 *   HeapFree(GetProcessHeap(), 0, buffer);
 *
 * @param n_bytes Size of the buffer is stored here.
 * @returns Pointer to the buffer or nullptr if out of memory.
 */
DLL_EXPORT LPVOID wlepToJpg(const std::wstring& wlep_filename, unsigned int* n_bytes);

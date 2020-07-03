// Standard Library
#include <iostream>
#include <string>
#include <thread>
// GDAL
#include <ogrsf_frmts.h>
// EXT
#include "nfd.h"

static const char* open_file(const char* extension = nullptr)
{
    nfdchar_t *filepath = NULL;
    nfdresult_t result = NFD_OpenDialog(extension, NULL, &filepath);
    if (result == NFD_OKAY)
    {
        // Successful read
        return filepath;
    }
    else if (result == NFD_CANCEL)
    {
        // User pressed cancel
        return nullptr;
    }
    else
    {
        // Error, run NFD_GetError()
        std::cerr << "Error: Could not read file!" << std::endl;
        return nullptr;
    }
}
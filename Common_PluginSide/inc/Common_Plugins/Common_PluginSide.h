#pragma once

#include "ACUPlugin.h"


void GrabPluginLoaderGlobalVariables(ACUPluginLoaderInterface& pluginLoader);
void RequestUnloadThisPlugin();


namespace fs = std::filesystem;
extern HMODULE g_ThisDLLHandle;
extern fs::path g_ThisDLLAbsoluteFilepath;
fs::path AbsolutePathInThisDLLDirectory(const fs::path& filenameRel);

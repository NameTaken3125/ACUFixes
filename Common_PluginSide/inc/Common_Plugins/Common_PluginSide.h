#pragma once

#include "ACUPlugin.h"

void GrabPluginLoaderGlobalVariables(ACUPluginLoaderInterface& pluginLoader);
void RequestUnloadThisPlugin();
extern HMODULE g_ThisDLLHandle;

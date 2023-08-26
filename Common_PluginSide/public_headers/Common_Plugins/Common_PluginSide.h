#pragma once

#include "ACUPlugin.h"


void GrabPluginLoaderGlobalVariables(ACUPluginLoaderInterface& pluginLoader);
void RequestUnloadThisPlugin();


namespace fs = std::filesystem;
extern HMODULE g_ThisDLLHandle;
extern fs::path g_ThisDLLAbsoluteFilepath;
fs::path AbsolutePathInThisDLLDirectory(const fs::path& filenameRel);

#define MAKE_VERSION_NUMBER_UINT64(major, minor, minorer, minorest) ((major << 24) | (minor << 16) | (minorer << 8) | (minorest))
class ACUPluginInterfaceVirtuals
{
public:
	virtual ~ACUPluginInterfaceVirtuals() {}
	virtual bool Start(ACUPluginLoaderInterface& pluginLoader) = 0;
	virtual uint64 GetThisPluginVersion() = 0;
	virtual void EveryFrameWhenMenuIsOpen();
	virtual void EveryFrameEvenWhenMenuIsClosed();
public:
	ACUPluginInterfaceVirtuals();
};

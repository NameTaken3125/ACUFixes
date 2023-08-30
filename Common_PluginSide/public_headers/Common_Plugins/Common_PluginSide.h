#pragma once

#include "ACUPlugin.h"

// If you call this, your plugin will be unloaded before the next frame.
// It is up to you to decide whether this option should be exposed to the user or not.
// Depending on what your plugin does, this might be fairly safe to do.
// I find this convenient while developing and testing.
// If you decide to export some functionality for interplugin operations (which I currently
// don't have any infrastructure for), then I strongly suggest that you
// _don't_ make this option available to users when you release your plugin.
void RequestUnloadThisPlugin();
// For the most basic interplugin operations.
HMODULE GetPluginIfLoaded(const wchar_t* pluginName);


extern HMODULE g_ThisDLLHandle;
extern std::filesystem::path g_ThisDLLAbsoluteFilepath;
std::filesystem::path AbsolutePathInThisDLLDirectory(const std::filesystem::path& filepathRelative);

#define MAKE_VERSION_NUMBER_UINT64(major, minor, minorer, minorest) ((major << 24) | (minor << 16) | (minorer << 8) | (minorest))
class ACUPluginInterfaceVirtuals
{
public:
	virtual ~ACUPluginInterfaceVirtuals() {}
	// Will be called immediately if the plugin loader confirms that your plugin API version is compatible.
	// Return `false` to unload the plugin.
	virtual bool Start(ACUPluginLoaderInterface& pluginLoader) = 0;
	// _Your_ plugin version. Currently is for logging only. (In the future, potentially for interplugin communications.)
	virtual uint64 GetThisPluginVersion() = 0;
	// Will be called every frame when the menu section for _your_ plugin is visible.
	virtual void EveryFrameWhenMenuIsOpen();
	// Will be called every frame.
	virtual void EveryFrameEvenWhenMenuIsClosed();
public:
	ACUPluginInterfaceVirtuals();
};

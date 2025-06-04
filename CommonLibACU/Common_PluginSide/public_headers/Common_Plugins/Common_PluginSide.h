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


extern HMODULE g_ThisDLLHandle;
extern std::filesystem::path g_ThisDLLAbsoluteFilepath;
std::filesystem::path AbsolutePathInThisDLLDirectory(const std::filesystem::path& filepathRelative);

#define MAKE_VERSION_NUMBER_UINT64(major, minor, minorer, minorest) ((major << 24) | (minor << 16) | (minorer << 8) | (minorest))
class ACUPluginInterfaceVirtuals
{
public:
	virtual ~ACUPluginInterfaceVirtuals() {}
	// This is probably where you want to initialize your plugin.
	// Will be called only if the plugin loader confirms that your plugin API version is compatible.
	// At this point the Main Integrity Check is killed, and the game's code can be safely patched.
	// Return `false` to unload the plugin.
	virtual bool InitStage_WhenCodePatchesAreSafeToApply(ACUPluginLoaderInterface& pluginLoader) = 0;
	// _Your_ plugin version. Currently is for logging only. (In the future, potentially for interplugin communications.)
	virtual uint64 GetThisPluginVersion() = 0;
	// Will be called every frame when the menu section for _your_ plugin is visible.
	virtual void EveryFrameWhenMenuIsOpen();
	// Will be called every frame.
	virtual void EveryFrameEvenWhenMenuIsClosed();
	// Will be called soon after loading if the PluginLoader determines your Plugin API version to be compatible.
	// It is okay to do stuff here that doesn't depend on the game, like initializing a log and reading a config file, for example.
	// It is not okay to try to patch the game's code, call the game's code, access the game's globals here.
	virtual void InitStage_WhenPluginAPIDeemedCompatible() {}


	// "Early Hooks" are the ones that run before the MainIntegrityCheck is killed,
	// before the game's window is opened, before it is safe to patch the game's code.
	// You probably don't need to use any of the Early Hooks in your plugin.
	// For example, none of them will be run
	// if you manually load the plugin from the ImGui menu.

	// You probably don't need to override this.
	// When this is called, the game's code is already unpacked,
	// the TypeInfos dictionary is filled out,
	// none of the game's objects have been loaded yet,
	// your plugin can call the game's native functions if it needs to,
	// but the Main Integrity Check isn't killed yet, so it is not safe
	// to patch the game's code.
	// Again, don't use if you don't need it.
	virtual void EarlyHook_WhenGameCodeIsUnpacked() {}
public:
	ACUPluginInterfaceVirtuals();
};

#pragma once

#include "ACU/basic_types.h"
#define MAKE_PLUGIN_LOADER_VERSION(major, minor, minorer, minorest) ((major << 24) | (minor << 16) | (minorer << 8) | (minorest))


constexpr uint64 g_CurrentPluginAPIversion = MAKE_PLUGIN_LOADER_VERSION(0, 9, 1, 0);


class ACUPluginLoaderSharedGlobals;
class ACUPluginLoaderInterface
{
public:
	uint64 m_PluginLoaderVersion = g_CurrentPluginAPIversion;
    // Call this if you want the PluginLoader to unload this DLL.
    void (*RequestUnloadPlugin)(HMODULE dllHandle) = nullptr;
	// Can be used for very basic interaction between plugins.
	HMODULE (*GetPluginIfLoaded)(const wchar_t* pluginName) = nullptr;
	ACUPluginLoaderSharedGlobals* m_ImplementationSharedVariables = nullptr;

	ACUPluginLoaderInterface(); // Defined in PluginLoader project.
};
assert_offsetof(ACUPluginLoaderInterface, m_PluginLoaderVersion, 0);
assert_offsetof(ACUPluginLoaderInterface, RequestUnloadPlugin, 8);
assert_offsetof(ACUPluginLoaderInterface, GetPluginIfLoaded, 0x10);


class ImGuiShared;
// Your plugin will fill this struct one time inside `ACUPluginStart()`.
struct ACUPluginInfo
{
	// The version of the plugin API that _your_ plugin is using. From `ACUPluginStart()`, set this to `g_CurrentPluginAPIversion`.
	uint64 m_PluginAPIVersion = -1;
	// _Your_ plugin version. Currently is for logging only. (In the future, potentially for interplugin communications.)
	uint64 m_PluginVersion;
	// Will be called when Main Integrity Check is disabled: this happens no sooner than the game's window appears.
	// You can safely install code patches from here.
	// Return `false` to unload the plugin.
	bool (*m_InitStage_WhenCodePatchesAreSafeToApply)(ACUPluginLoaderInterface& pluginLoader) = nullptr;
	// Will be called every frame when the menu section for _your_ plugin is visible.
	void (*m_EveryFrameWhenMenuIsOpen)(ImGuiShared& readyToUseImGuiContext) = nullptr;
	// Will be called every frame.
	void (*m_EveryFrameEvenWhenMenuIsClosed)(ImGuiShared& readyToUseImGuiContext) = nullptr;

	// After `ACUPluginStart()` has been run and the PluginLoader has received your Plugin API version,
	// and has determined that your Plugin API version looks compatible, this will be called immediately.
	// This can be very early during game load. It's not safe to apply normal code patches
	// or call game's functions from here.
	void (*m_InitStage_WhenVersionsAreDeemedCompatible)(ACUPluginLoaderInterface& pluginLoader) = nullptr;

	// "Early Hooks" are the ones that run before the MainIntegrityCheck is killed,
	// before the game's window is opened, before it is safe to patch the game's code.
	// You probably don't need to use any of the early hooks in your plugin.
	// For example, none of the Early Hooks will be run
	// if you manually load the plugin from the ImGui menu.

	void (*m_EarlyHook_WhenGameCodeIsUnpacked)() = nullptr;
};
assert_offsetof(ACUPluginInfo, m_PluginAPIVersion, 0);
assert_offsetof(ACUPluginInfo, m_PluginVersion, 8);
assert_offsetof(ACUPluginInfo, m_InitStage_WhenCodePatchesAreSafeToApply, 0x10);
assert_offsetof(ACUPluginInfo, m_EveryFrameWhenMenuIsOpen, 0x18);
assert_offsetof(ACUPluginInfo, m_EveryFrameEvenWhenMenuIsClosed, 0x20);

// PluginLoader will call this immediately after loading your plugin.
extern "C" __declspec(dllexport) bool ACUPluginStart(ACUPluginLoaderInterface& pluginLoader, ACUPluginInfo& yourPluginInfo_out);

#pragma once

#include "ACU/basic_types.h"
#define MAKE_PLUGIN_LOADER_VERSION(major, minor, minorer, minorest) ((major << 24) | (minor << 16) | (minorer << 8) | (minorest))


constexpr uint64 g_CurrentPluginAPIversion = MAKE_PLUGIN_LOADER_VERSION(0, 9, 0, 0);


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
struct ACUPluginInfo
{
	// The version of the plugin API that _your_ plugin is using. From `ACUPluginStart()`, set this to `g_CurrentPluginAPIversion`.
	uint64 m_PluginAPIVersion = -1;
	// _Your_ plugin version. Currently is for logging only. (In the future, potentially for interplugin communications.)
	uint64 m_PluginVersion;
	// Will be called immediately if the plugin loader confirms that your plugin API version is compatible.
	// Return `false` to unload the plugin.
	bool (*m_Start)(ACUPluginLoaderInterface& pluginLoader) = nullptr;
	// Will be called every frame when the menu section for _your_ plugin is visible.
	void (*m_EveryFrameWhenMenuIsOpen)(ImGuiShared& readyToUseImGuiContext) = nullptr;
	// Will be called every frame.
	void (*m_EveryFrameEvenWhenMenuIsClosed)(ImGuiShared& readyToUseImGuiContext) = nullptr;
};
assert_offsetof(ACUPluginInfo, m_PluginAPIVersion, 0);
assert_offsetof(ACUPluginInfo, m_PluginVersion, 8);
assert_offsetof(ACUPluginInfo, m_Start, 0x10);
assert_offsetof(ACUPluginInfo, m_EveryFrameWhenMenuIsOpen, 0x18);
assert_offsetof(ACUPluginInfo, m_EveryFrameEvenWhenMenuIsClosed, 0x20);

extern "C" __declspec(dllexport) bool ACUPluginStart(ACUPluginLoaderInterface& pluginLoader, ACUPluginInfo& yourPluginInfo_out);

#pragma once

#include "ACU/basic_types.h"
#define MAKE_PLUGIN_LOADER_VERSION(major, minor, minorer, minorest) ((major << 24) | (minor << 16) | (minorer << 8) | (minorest))


constexpr uint64 g_CurrentPluginAPIversion = MAKE_PLUGIN_LOADER_VERSION(0, 8, 0, 0);


struct ACUPluginLoaderInterface;
struct ImGuiContext;
class ACUPluginInterfaceVirtuals
{
public:
	virtual ~ACUPluginInterfaceVirtuals() {}
	virtual bool Start(ACUPluginLoaderInterface& pluginLoader) = 0;
	virtual void EveryFrameWhenMenuIsOpen(ImGuiContext& readyToUseImGuiContext);
	virtual void EveryFrameEvenWhenMenuIsClosed(ImGuiContext& readyToUseImGuiContext);
public:
	ACUPluginInterfaceVirtuals();
};
class ACUPluginLoaderSharedGlobals;
struct ACUPluginLoaderInterface
{
	uint64 m_PluginLoaderVersion = g_CurrentPluginAPIversion;
    // Call this if you want the PluginLoader to unload this DLL.
    void (*RequestUnloadPlugin)(HMODULE dllHandle) = nullptr;
	// Can be used for very basic interaction between plugins.
	HMODULE (*GetPluginIfLoaded)(const wchar_t* pluginName) = nullptr;
	ACUPluginLoaderSharedGlobals* m_ImplementationSharedVariables = nullptr;
};
assert_offsetof(ACUPluginLoaderInterface, m_PluginLoaderVersion, 0);
assert_offsetof(ACUPluginLoaderInterface, RequestUnloadPlugin, 8);
assert_offsetof(ACUPluginLoaderInterface, GetPluginIfLoaded, 0x10);

// This function will be called from the Plugin Loader after loading the plugin DLL.
extern "C" __declspec(dllexport) ACUPluginInterfaceVirtuals* ACUPluginStart(ACUPluginLoaderInterface& pluginLoader);

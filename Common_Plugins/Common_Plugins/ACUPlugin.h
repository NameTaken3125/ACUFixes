#pragma once

class ACUPluginInterfaceVirtuals
{
public:
	virtual ~ACUPluginInterfaceVirtuals() {}
	virtual void EveryFrameWhenMenuIsOpen(ImGuiContext& readyToUseImGuiContext) = 0;
	virtual void EveryFrameEvenWhenMenuIsClosed(ImGuiContext& readyToUseImGuiContext) = 0;
};


struct ACUPluginLoaderInterface
{
    // Call this if you want the PluginLoader to unload this DLL.
    void (*RequestUnloadPlugin)(HMODULE dllHandle);
};

extern "C" __declspec(dllexport) ACUPluginInterfaceVirtuals* ACUPluginStart(ACUPluginLoaderInterface & pluginLoader);

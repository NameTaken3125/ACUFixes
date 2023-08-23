#include "pch.h"
#include "Common_Plugins/Common_PluginSide.h"

static ACUPluginInterfaceVirtuals* g_ThisPluginSingletonAsBaseclass = nullptr;
ACUPluginInterfaceVirtuals::ACUPluginInterfaceVirtuals()
{
	g_ThisPluginSingletonAsBaseclass = this;
}
void ACUPluginInterfaceVirtuals::EveryFrameWhenMenuIsOpen(ImGuiContext& readyToUseImGuiContext) {}
void ACUPluginInterfaceVirtuals::EveryFrameEvenWhenMenuIsClosed(ImGuiContext& readyToUseImGuiContext) {}







using RequestUnloadPlugin_fnt = decltype(ACUPluginLoaderInterface::RequestUnloadPlugin);
RequestUnloadPlugin_fnt RequestUnloadThisPlugin_fnptr = nullptr;
void RequestUnloadThisPlugin()
{
    RequestUnloadThisPlugin_fnptr(g_ThisDLLHandle);
}
void GrabPluginLoaderGlobalVariables(ACUPluginLoaderInterface& pluginLoader)
{
    RequestUnloadThisPlugin_fnptr = pluginLoader.RequestUnloadPlugin;
}









extern "C" __declspec(dllexport) ACUPluginInterfaceVirtuals* ACUPluginStart(ACUPluginLoaderInterface& pluginLoader)
{
    if (!g_ThisPluginSingletonAsBaseclass)
    {
        return nullptr;
    }
    if (!g_ThisPluginSingletonAsBaseclass->Start(pluginLoader))
    {
        return nullptr;
    }
    return g_ThisPluginSingletonAsBaseclass;
}
HMODULE g_ThisDLLHandle = nullptr;
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_ThisDLLHandle = hModule;
        DisableThreadLibraryCalls(hModule);
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    default:
        break;
    }
    return TRUE;
}

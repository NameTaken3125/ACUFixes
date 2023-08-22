/*
main.cpp
Brings the app's components together.
*/

#include "pch.h"

#include "base.h"
#include "MyVariousHacks.h"
#include "MyLog.h"
#include "MainConfig.h"


// Just re-declaring these as a reminder that they're the user-defined parts
// that are needed to make the BaseHook work.
void Base::ImGuiLayer_EvenWhenMenuIsClosed();
void Base::ImGuiLayer_WhenMenuIsOpen();
fs::path AbsolutePathInMyDirectory(const fs::path& filenameRel);

#include "Common_Plugins/ACUPlugin.h"


HMODULE g_ThisDLLHandle = nullptr;
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

extern ImGuiContext* GImGui;
std::optional<MyLogFileLifetime> g_LogLifetime;
class ACUFixes_TheFixesPlugin : public ACUPluginInterfaceVirtuals
{
public:
    virtual void EveryFrameWhenMenuIsOpen(ImGuiContext& readyToUseImGuiContext) override
    {
        GImGui = &readyToUseImGuiContext;
        Base::ImGuiLayer_WhenMenuIsOpen();
    }
    virtual void EveryFrameEvenWhenMenuIsClosed(ImGuiContext& readyToUseImGuiContext) override
    {
        GImGui = &readyToUseImGuiContext;
        Base::ImGuiLayer_EvenWhenMenuIsClosed();
    }
    virtual bool Start(ACUPluginLoaderInterface& pluginLoader) override
    {
        if (pluginLoader.m_PluginLoaderVersion < g_CurrentPluginAPIversion)
        {
            return false;
        }
        GrabPluginLoaderGlobalVariables(pluginLoader);

        g_LogLifetime.emplace(AbsolutePathInMyDirectory("ACUFixes-log.log"));
        MainConfig::FindAndLoadConfigFileOrCreateDefault();
        MyVariousHacks::Start();
        return true;
    }
} g_thisPlugin;
extern "C" __declspec(dllexport) ACUPluginInterfaceVirtuals* ACUPluginStart(ACUPluginLoaderInterface& pluginLoader)
{
    if (!g_ThisPluginSingletonAsBaseclass->Start(pluginLoader))
    {
        return nullptr;
    }
    return g_ThisPluginSingletonAsBaseclass;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_ThisDLLHandle = hModule;
        Base::Data::thisDLLModule = hModule;
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
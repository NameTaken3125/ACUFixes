#include "pch.h"
#include "Common_Plugins/Common_PluginSide.h"

static ACUPluginInterfaceVirtuals* g_ThisPluginSingletonAsBaseclass = nullptr;
void** g_PluginInterfaceBaseclassVTable = nullptr;
ACUPluginInterfaceVirtuals::ACUPluginInterfaceVirtuals()
{
    g_PluginInterfaceBaseclassVTable = *(void***)this;
	g_ThisPluginSingletonAsBaseclass = this;
}
void ACUPluginInterfaceVirtuals::EveryFrameWhenMenuIsOpen(ImGuiContext& readyToUseImGuiContext) {}
void ACUPluginInterfaceVirtuals::EveryFrameEvenWhenMenuIsClosed(ImGuiContext& readyToUseImGuiContext) {}







decltype(ACUPluginLoaderInterface::RequestUnloadPlugin) RequestUnloadThisPlugin_fnptr = nullptr;
decltype(ACUPluginLoaderInterface::GetPluginIfLoaded) GetPluginIfLoaded_fnptr = nullptr;
void RequestUnloadThisPlugin()
{
    RequestUnloadThisPlugin_fnptr(g_ThisDLLHandle);
}
#include "Common_Plugins_impl/InputHooks.h"
#include "Common_Plugins_impl/PluginLoaderSharedGlobals.h"
void GrabPluginLoaderGlobalVariables(ACUPluginLoaderInterface& pluginLoader)
{
    RequestUnloadThisPlugin_fnptr = pluginLoader.RequestUnloadPlugin;
    ACU::Input::g_InputHooksSingletonPtr = &pluginLoader.m_ImplementationSharedVariables->m_InputHooks;
}








static bool IsPluginInterfaceVirtualFunctionOverridden(int virtualFnIdx)
{
    void** pluginVTable = *(void***)g_ThisPluginSingletonAsBaseclass;
    return g_PluginInterfaceBaseclassVTable[virtualFnIdx] != pluginVTable[virtualFnIdx];
}
static bool EverythingAppearsToBeReadyToStart(ACUPluginLoaderInterface& pluginLoader)
{
    GrabPluginLoaderGlobalVariables(pluginLoader);
    return g_ThisPluginSingletonAsBaseclass->Start(pluginLoader);
}
extern "C" __declspec(dllexport) bool ACUPluginStart(ACUPluginLoaderInterface& pluginLoader, ACUPluginInfo& yourPluginInfo_out)
{
    if (pluginLoader.m_PluginLoaderVersion < g_CurrentPluginAPIversion) {
        return false;
    }
    if (!g_ThisPluginSingletonAsBaseclass) {
        return false;
    }


    yourPluginInfo_out.m_PluginAPIVersion = g_CurrentPluginAPIversion;
    yourPluginInfo_out.m_PluginVersion = g_ThisPluginSingletonAsBaseclass->GetThisPluginVersion();


    constexpr int vfnIdx_EveryFrameWhenMenuIsOpen = 3;
    constexpr int vfnIdx_EveryFrameEvenWhenMenuIsClosed = 4;
    if (IsPluginInterfaceVirtualFunctionOverridden(vfnIdx_EveryFrameWhenMenuIsOpen)) {
        yourPluginInfo_out.m_EveryFrameWhenMenuIsOpen = [](ImGuiContext& readyToUseImGuiContext) {
            g_ThisPluginSingletonAsBaseclass->EveryFrameWhenMenuIsOpen(readyToUseImGuiContext);
        };
    }
    if (IsPluginInterfaceVirtualFunctionOverridden(vfnIdx_EveryFrameEvenWhenMenuIsClosed)) {
        yourPluginInfo_out.m_EveryFrameEvenWhenMenuIsClosed = [](ImGuiContext& readyToUseImGuiContext) {
            g_ThisPluginSingletonAsBaseclass->EveryFrameEvenWhenMenuIsClosed(readyToUseImGuiContext);
        };
    }
    yourPluginInfo_out.m_Start = EverythingAppearsToBeReadyToStart;
    return true;
}


HMODULE g_ThisDLLHandle = nullptr;
fs::path g_ThisDLLAbsoluteFilepath;
fs::path GetDLLAbsolutePath(HMODULE dllHandle)
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(dllHandle, path, (DWORD)std::size(path));
    return fs::path(path);
}
fs::path AbsolutePathInThisDLLDirectory(const fs::path& filenameRel)
{
    fs::path& dll = g_ThisDLLAbsoluteFilepath;
    fs::path fullPath = dll.parent_path() / filenameRel;
    return fullPath;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_ThisDLLHandle = hModule;
        g_ThisDLLAbsoluteFilepath = GetDLLAbsolutePath(hModule);
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

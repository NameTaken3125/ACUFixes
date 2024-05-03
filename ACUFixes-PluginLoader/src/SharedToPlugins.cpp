#include "pch.h"


#include "Common_Plugins_impl/PluginLoaderSharedGlobals.h"
#include "Common_Plugins_impl/InputHooks.h"
#include "Common_Plugins_impl/AnimationModdingInterface.h"
#include "Common_Plugins/ACUPlugin.h"
extern ACU::Input::InputHooks g_InputHooks;
extern AnimationModdingInterface g_AnimationModding;
ACUPluginLoaderSharedGlobals g_SharedVariables;
ACUPluginLoaderSharedGlobals::ACUPluginLoaderSharedGlobals()
    : m_InputHooks(g_InputHooks)
    , m_AnimationModding(g_AnimationModding)
{}

HMODULE PluginLoader_GetPluginIfLoaded(const wchar_t* pluginName);
void PluginLoader_RequestUnloadPlugin(HMODULE dllHandle);
ACUPluginLoaderInterface::ACUPluginLoaderInterface()
{
    RequestUnloadPlugin = ::PluginLoader_RequestUnloadPlugin;
    GetPluginIfLoaded = ::PluginLoader_GetPluginIfLoaded;
    m_ImplementationSharedVariables = &g_SharedVariables;
}

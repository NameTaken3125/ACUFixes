#include "pch.h"


#include "Common_Plugins_impl/PluginLoaderSharedGlobals.h"
#include "Common_Plugins_impl/InputHooks.h"
#include "Common_Plugins_impl/AnimationModdingInterface.h"
#include "Common_Plugins_impl/ImGuiConsoleInterface.h"
#include "Common_Plugins/ACUPlugin.h"
extern ACU::Input::InputHooks g_InputHooks;
template<typename Cls> Cls& GetPluginInterfaceSingleton() { static Cls singleton; return singleton; }
ACUPluginLoaderSharedGlobals::ACUPluginLoaderSharedGlobals()
    : m_InputHooks(g_InputHooks)
    , m_AnimationModding(GetPluginInterfaceSingleton<AnimationModdingInterface>())
    , m_ImGuiConsole(GetPluginInterfaceSingleton<ImGuiConsoleInterface>())
{}

HMODULE PluginLoader_GetPluginIfLoaded(const wchar_t* pluginName);
void PluginLoader_RequestUnloadPlugin(HMODULE dllHandle);
#include "PluginLoader_VariousHooks/Animation/AnimationGraphEvaluationPatches.h"
ACUPluginLoaderInterface::ACUPluginLoaderInterface()
{
    RequestUnloadPlugin = ::PluginLoader_RequestUnloadPlugin;
    GetPluginIfLoaded = ::PluginLoader_GetPluginIfLoaded;
    // To prevent the "destruction order fiasco", I make sure objects
    // used in the plugin interface are fully constructed before the interface is,
    // meaning they shouldn't be destroyed until the interface is
    // (which is only supposed to happen if "Uninject all plugins and PluginLoader" is used)
    // (which normally isn't supposed to happen at all - the PluginLoader isn't normally supposed to be uninjected,
    // but when developing the PluginLoader itself its uninjectability is still desirable)
    m_ImplementationSharedVariables = &GetPluginInterfaceSingleton<ACUPluginLoaderSharedGlobals>();
}

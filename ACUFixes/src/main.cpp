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

#include "Common_Plugins/Common_PluginSide.h"



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

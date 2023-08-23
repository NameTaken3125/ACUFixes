/*
main.cpp
Brings the app's components together.
*/

#include "pch.h"

#include "MyVariousHacks.h"
#include "MyLog.h"
#include "MainConfig.h"

#include "Common_Plugins/Common_PluginSide.h"


extern ImGuiContext* GImGui;
std::optional<MyLogFileLifetime> g_LogLifetime;
void ImGuiLayer_EvenWhenMenuIsClosed();
void ImGuiLayer_WhenMenuIsOpen();
class ACUFixes_TheFixesPlugin : public ACUPluginInterfaceVirtuals
{
public:
    virtual void EveryFrameWhenMenuIsOpen(ImGuiContext& readyToUseImGuiContext) override
    {
        GImGui = &readyToUseImGuiContext;
        ImGuiLayer_WhenMenuIsOpen();
    }
    virtual void EveryFrameEvenWhenMenuIsClosed(ImGuiContext& readyToUseImGuiContext) override
    {
        GImGui = &readyToUseImGuiContext;
        ImGuiLayer_EvenWhenMenuIsClosed();
    }
    virtual bool Start(ACUPluginLoaderInterface& pluginLoader) override
    {
        if (pluginLoader.m_PluginLoaderVersion < g_CurrentPluginAPIversion)
        {
            return false;
        }
        GrabPluginLoaderGlobalVariables(pluginLoader);

        g_LogLifetime.emplace(AbsolutePathInThisDLLDirectory("ACUFixes-log.log"));
        MainConfig::FindAndLoadConfigFileOrCreateDefault(AbsolutePathInThisDLLDirectory("ACUFixes-config.json"));
        MyVariousHacks::Start();
        return true;
    }
} g_thisPlugin;

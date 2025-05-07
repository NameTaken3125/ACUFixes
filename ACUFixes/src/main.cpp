/*
main.cpp
Brings the app's components together.
*/

#include "pch.h"

#include "MyVariousHacks.h"
#include "MyLog.h"
#include "MainConfig.h"

#include "Common_Plugins/Common_PluginSide.h"


std::optional<MyLogFileLifetime> g_LogLifetime;
void ImGuiLayer_EvenWhenMenuIsClosed();
void ImGuiLayer_WhenMenuIsOpen();
void ApplyAnimationGraphMods();
void HacksContainer_AssetOverrides_Start();
class ACUFixes_TheFixesPlugin : public ACUPluginInterfaceVirtuals
{
public:
    virtual void EveryFrameWhenMenuIsOpen() override
    {
        ImGuiLayer_WhenMenuIsOpen();
    }
    virtual void EveryFrameEvenWhenMenuIsClosed() override
    {
        ImGuiLayer_EvenWhenMenuIsClosed();
    }
    virtual uint64 GetThisPluginVersion() override
    {
        return MAKE_VERSION_NUMBER_UINT64(0, 0, 1, 0);
    }
    virtual bool Start(ACUPluginLoaderInterface& pluginLoader) override
    {
        g_LogLifetime.emplace(AbsolutePathInThisDLLDirectory(THIS_DLL_PROJECT_NAME "-log.log"));
        MainConfig::FindAndLoadConfigFileOrCreateDefault(AbsolutePathInThisDLLDirectory(THIS_DLL_PROJECT_NAME "-config.json"));
        MyVariousHacks::Start();
        HacksContainer_AssetOverrides_Start();
        ApplyAnimationGraphMods();
        return true;
    }
} g_thisPlugin;

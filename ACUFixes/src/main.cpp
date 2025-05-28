/*
main.cpp
Brings the app's components together.
*/

#include "pch.h"

#include "MyVariousHacks.h"
#include "MyLog.h"
#include "MainConfig.h"

#include "Common_Plugins/Common_PluginSide.h"

#define LOG_FILENAME    THIS_DLL_PROJECT_NAME "-log.log"
#define CONFIG_FILENAME THIS_DLL_PROJECT_NAME "-config.json"

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
    virtual void InitStage_WhenPluginAPIDeemedCompatible() override
    {
        g_LogLifetime.emplace(AbsolutePathInThisDLLDirectory(LOG_FILENAME));
        MainConfig::FindAndLoadConfigFileOrCreateDefault(AbsolutePathInThisDLLDirectory(CONFIG_FILENAME));
    }
    virtual void InitStage_WhenGameCodeIsUnpacked() override
    {
        LOG_DEBUG(DefaultLogger,
            "Early hook: InitStage_WhenGameCodeIsUnpacked()\n"
        );
        void AssetOverrides_EarlyHooks_Start(); AssetOverrides_EarlyHooks_Start();
        void AssetOverrides_InitFromLoadOrder_EarlyHook(); AssetOverrides_InitFromLoadOrder_EarlyHook();
    }
    virtual bool InitStage_WhenCodePatchesAreSafeToApply(ACUPluginLoaderInterface& pluginLoader) override
    {
        MyVariousHacks::Start();
        void AssetOverrides_EarlyHooks_Start_End(); AssetOverrides_EarlyHooks_Start_End();
        HacksContainer_AssetOverrides_Start();
        ApplyAnimationGraphMods();
        return true;
    }
} g_thisPlugin;

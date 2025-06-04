/*
main.cpp
Brings the app's components together.
*/

#include "pch.h"

#include "MyVariousHacks.h"
#include "MyLog.h"
#include "MainConfig.h"
#include "Handles.h"

#include "Common_Plugins/Common_PluginSide.h"

#define LOG_FILENAME    THIS_DLL_PROJECT_NAME "-log.log"
#define CONFIG_FILENAME THIS_DLL_PROJECT_NAME "-config.json"

std::optional<MyLogFileLifetime> g_LogLifetime;
void ImGuiLayer_EvenWhenMenuIsClosed();
void ImGuiLayer_WhenMenuIsOpen();
void ApplyAnimationGraphMods();
class AssetOverridesPlugin : public ACUPluginInterfaceVirtuals
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
        ACU::Handles::LoadHandlesmapFile();
    }
    virtual void EarlyHook_WhenGameCodeIsUnpacked() override
    {
    }
    virtual bool InitStage_WhenCodePatchesAreSafeToApply(ACUPluginLoaderInterface& pluginLoader) override
    {
        MyVariousHacks::Start();
        ApplyAnimationGraphMods();
        return true;
    }
} g_thisPlugin;

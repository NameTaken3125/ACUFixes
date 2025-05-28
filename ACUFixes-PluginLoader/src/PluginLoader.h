#pragma once


#include "Common_Plugins/ACUPlugin.h"
using ACUPluginStart_fnt = decltype(ACUPluginStart)*;
struct MyPluginResult
{
    MyPluginResult(const fs::path& filepath)
        : m_filepath(filepath)
        , m_PluginName(filepath.filename().u8string())
    {}
    const fs::path m_filepath;
    const std::string m_PluginName;
    struct SuccessfulLoad
    {
        SuccessfulLoad(HMODULE moduleHandle, std::unique_ptr<ACUPluginInfo>&& pluginInterface)
            : m_moduleHandle(moduleHandle)
            , m_pluginInterface(std::move(pluginInterface))
        {
        }
        HMODULE m_moduleHandle;
        std::unique_ptr<ACUPluginInfo> m_pluginInterface;
        bool m_isRequestedToUnload = false;
        bool m_isMenuOpen = false;
    };
    std::optional<SuccessfulLoad> m_successfulLoad;
};
using PluginLoaderResults = std::vector<std::unique_ptr<MyPluginResult>>;
class MyPluginLoader
{
public:
    void UpdateListOfAvailablePlugins();
    void LoadAllFoundNonloadedPlugins();
    void LoadAllFoundNonloadedPluginsAndCheckCompatibility();
    void WhenGameCodeIsUnpacked();
    void WhenSafeToApplyCodePatches();
    void UnloadAllPlugins();
    void DrawPluginListControls();
    void DrawImGuiForPlugins_WhenMenuIsOpened();
    void DrawImGuiForPlugins_EvenWhenMenuIsClosed();
    ~MyPluginLoader() { UnloadAllPlugins(); }

    void RequestUnloadDLL(HMODULE dllHandle);
    HMODULE GetPluginIfLoaded(const wchar_t* pluginName);
private:
    void LoadAndStartPlugin(MyPluginResult& pluginRecord);
    void LoadPluginAndCheckCompatibility(MyPluginResult& pluginRecord);
public:
    void EveryFrameBeforeGraphicsUpdate();
private:
    PluginLoaderResults dllResults;
    bool m_IsRequestedToUnloadPlugin = false;
    ACUPluginLoaderInterface m_PluginLoaderInterfaces;
};

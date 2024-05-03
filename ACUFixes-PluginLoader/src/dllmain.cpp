#include "pch.h"

#include "base.h"
#include "MyLog.h"
#include "PluginLoaderConfig.h"
#include "PresentHookOuter.h"


// Required so that the game doesn't crash on code modification.
void DisableMainIntegrityCheck();
void WaitUntilGameIsInitializedEnoughSoThatTheMainIntegrityCheckCanBeDisabled();
void ClearThe_BeingDebugged_Flag();
// Respond to input: toggle hacks, etc.
LRESULT CALLBACK WndProc_HackControls(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


class PresentHookOuter::BasehookSettings_PresentHookOuter : public Base::Settings
{
public:
    BasehookSettings_PresentHookOuter() : Base::Settings(false, WndProc_HackControls) {}
    virtual void OnBeforeActivate() override {
        PresentHookOuter::Activate();
        Base::Data::ShowMenu = false;
    }
    virtual void OnBeforeDetach() override {
        PresentHookOuter::Deactivate();
    }
};
fs::path AbsolutePathInMyDirectory(const fs::path& filenameRel);

#include "Common_Plugins/ACUPlugin.h"
using ACUPluginStart_fnt = decltype(ACUPluginStart)*;
struct MyPluginResult
{
    MyPluginResult(const fs::path& filepath) : m_filepath(filepath) {}
    const fs::path m_filepath;
    struct SuccessfulLoad
    {
        SuccessfulLoad(HMODULE moduleHandle, std::unique_ptr<ACUPluginInfo>&& pluginInterface)
            : m_moduleHandle(moduleHandle)
            , m_pluginInterface(std::move(pluginInterface))
        {}
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
    void UnloadAllPlugins();
    void DrawPluginListControls();
    void DrawImGuiForPlugins_WhenMenuIsOpened();
    void DrawImGuiForPlugins_EvenWhenMenuIsClosed();
    ~MyPluginLoader() { UnloadAllPlugins(); }

    void RequestUnloadDLL(HMODULE dllHandle);
    HMODULE GetPluginIfLoaded(const wchar_t* pluginName);
private:
    void LoadAndStartPlugin(MyPluginResult& pluginRecord);
public:
    void EveryFrameBeforeGraphicsUpdate();
private:
    PluginLoaderResults dllResults;
    bool m_IsRequestedToUnloadPlugin = false;
    ACUPluginLoaderInterface m_PluginLoaderInterfaces;
} g_MyPluginLoader;
bool IsPluginAPIversionCompatible(uint64 pluginLoaderVersion, uint64 pluginVersion)
{
    // If the pluginloader updates introduce changes that would break old plugins,
    // I can list the breaking versions right here.
    return pluginLoaderVersion >= pluginVersion;
}
#define PLUGIN_API_VERSION_GET_MAJOR(version) ((version >> 24) & 0xFF)
#define PLUGIN_API_VERSION_GET_MINOR(version) ((version >> 16) & 0xFF)
#define PLUGIN_API_VERSION_GET_MINORER(version) ((version >> 8) & 0xFF)
#define PLUGIN_API_VERSION_GET_MINOREST(version) (version & 0xFF)
void MyPluginLoader::LoadAndStartPlugin(MyPluginResult& pluginRecord)
{
    const auto* pluginFilepath = pluginRecord.m_filepath.c_str();
    fs::path _pluginName = pluginRecord.m_filepath.filename().c_str();
    const auto* pluginName = _pluginName.c_str();
    LOG_DEBUG(L"[*] Plugin %s: Trying to load.\n", pluginFilepath);
    HMODULE moduleHandle = LoadLibraryW(pluginFilepath);
    if (!moduleHandle)
    {
        LOG_DEBUG(L"[x] Plugin %s: Failed to load.\n", pluginName);
        return;
    }
    LOG_DEBUG(L"[*] Plugin %s: handle: %llX\n", pluginName, moduleHandle);
    ACUPluginStart_fnt startFn = reinterpret_cast<ACUPluginStart_fnt>(GetProcAddress(moduleHandle, "ACUPluginStart"));
    if (!startFn)
    {
        LOG_DEBUG(L"[x] Plugin %s: Doesn't export the `ACUPluginStart()` function. Unloading.\n", pluginName);
        FreeLibrary(moduleHandle);
        return;
    }
    LOG_DEBUG(L"[*] Plugin %s: Address of the `ACUPluginStart()` function: %llX\n", pluginName, startFn);
    std::unique_ptr<ACUPluginInfo> pluginInfo = std::make_unique<ACUPluginInfo>();
    bool isPluginThinksIsReadyToStart = startFn(m_PluginLoaderInterfaces, *pluginInfo);
    if (!isPluginThinksIsReadyToStart)
    {
        LOG_DEBUG(L"[x] Plugin %s: `ACUPluginStart()` returned `false`. Unloading.\n", pluginName);
        FreeLibrary(moduleHandle);
        return;
    }
    LOG_DEBUG(L"[*] Plugin %s: `ACUPluginStart()` returned `true`. Plugin API version: %d.%d.%d.%d. Plugin's version: 0x%llX.\n"
        , pluginName
        , PLUGIN_API_VERSION_GET_MAJOR(pluginInfo->m_PluginAPIVersion)
        , PLUGIN_API_VERSION_GET_MINOR(pluginInfo->m_PluginAPIVersion)
        , PLUGIN_API_VERSION_GET_MINORER(pluginInfo->m_PluginAPIVersion)
        , PLUGIN_API_VERSION_GET_MINOREST(pluginInfo->m_PluginAPIVersion)
        , pluginInfo->m_PluginVersion
    );
    if (!IsPluginAPIversionCompatible(g_CurrentPluginAPIversion, pluginInfo->m_PluginAPIVersion))
    {
        LOG_DEBUG(L"[x] Plugin %s: Plugin's API version %d.%d.%d.%d is incompatible with the version of this plugin loader (%d.%d.%d.%d). Unloading.\n"
            , pluginName
            , PLUGIN_API_VERSION_GET_MAJOR(pluginInfo->m_PluginAPIVersion)
            , PLUGIN_API_VERSION_GET_MINOR(pluginInfo->m_PluginAPIVersion)
            , PLUGIN_API_VERSION_GET_MINORER(pluginInfo->m_PluginAPIVersion)
            , PLUGIN_API_VERSION_GET_MINOREST(pluginInfo->m_PluginAPIVersion)
            , PLUGIN_API_VERSION_GET_MAJOR(g_CurrentPluginAPIversion)
            , PLUGIN_API_VERSION_GET_MINOR(g_CurrentPluginAPIversion)
            , PLUGIN_API_VERSION_GET_MINORER(g_CurrentPluginAPIversion)
            , PLUGIN_API_VERSION_GET_MINOREST(g_CurrentPluginAPIversion)
            );
        FreeLibrary(moduleHandle);
        return;
    }
    if (!pluginInfo->m_Start)
    {
        LOG_DEBUG(L"[x] Plugin %s: Plugin didn't provide the `ACUPluginInfo.m_Start()` callback. Unloading.\n", pluginName);
        FreeLibrary(moduleHandle);
        return;
    }
    if (!pluginInfo->m_Start(m_PluginLoaderInterfaces))
    {
        LOG_DEBUG(L"[x] Plugin %s: The start procedure returned `false` indicating that the plugin doesn't want to start. Unloading.\n", pluginName);
        FreeLibrary(moduleHandle);
        return;
    }
    LOG_DEBUG(L"[+] Plugin %s: Successfully loaded.\n", pluginName);
    pluginRecord.m_successfulLoad.emplace(moduleHandle, std::move(pluginInfo));
}
#undef PLUGIN_API_VERSION_GET_MAJOR
#undef PLUGIN_API_VERSION_GET_MINOR
#undef PLUGIN_API_VERSION_GET_MINORER
#undef PLUGIN_API_VERSION_GET_MINOREST
void MyPluginLoader::UnloadAllPlugins()
{
    for (std::unique_ptr<MyPluginResult>& dll : dllResults)
    {
        if (dll->m_successfulLoad)
        {
            FreeLibrary(dll->m_successfulLoad->m_moduleHandle);
            dll->m_successfulLoad.reset();
        }
    }
}
void MyPluginLoader::RequestUnloadDLL(HMODULE dllHandle)
{
    for (std::unique_ptr<MyPluginResult>& loadedPlugin : dllResults)
    {
        if (loadedPlugin->m_successfulLoad
            && loadedPlugin->m_successfulLoad->m_moduleHandle == dllHandle)
        {
            LOG_DEBUG(L"[*] Plugin %s: Requested to unload.\n", loadedPlugin->m_filepath.filename().c_str());
            loadedPlugin->m_successfulLoad->m_isRequestedToUnload = true;
            m_IsRequestedToUnloadPlugin = true;
            break;
        }
    }
}

HMODULE PluginLoader_GetPluginIfLoaded(const wchar_t* pluginName)
{
    return g_MyPluginLoader.GetPluginIfLoaded(pluginName);
}
void PluginLoader_RequestUnloadPlugin(HMODULE dllHandle)
{
    g_MyPluginLoader.RequestUnloadDLL(dllHandle);
}
HMODULE MyPluginLoader::GetPluginIfLoaded(const wchar_t* pluginName)
{
    for (auto& plugin : dllResults)
    {
        if (!plugin->m_successfulLoad) { continue; }
        if (plugin->m_filepath.filename() == pluginName)
        {
            return plugin->m_successfulLoad->m_moduleHandle;
        }
    }
    return nullptr;
}
void MyPluginLoader::UpdateListOfAvailablePlugins()
{
    LOG_DEBUG(L"[*] UpdateListOfAvailablePlugins():\n");
    fs::path pluginsFolder = AbsolutePathInMyDirectory(L"plugins");
    fs::create_directory(pluginsFolder);
    std::vector<fs::path> currentlyPresentDLLsInFolder;
    currentlyPresentDLLsInFolder.reserve(32);
    for (const auto& entry : std::filesystem::directory_iterator(pluginsFolder)) {
        if (entry.path().extension() == L".dll") {
            currentlyPresentDLLsInFolder.push_back(entry.path());
            LOG_DEBUG(L"[*] Found DLL: %s\n", entry.path().c_str());
        }
    }
    // Remove the records of DLLs that were available but are not anymore.
    auto IsPluginRecordPresentButDLLisNoLongerAvailable = [&](std::unique_ptr<MyPluginResult>& alreadyDetectedPlugin)
    {
        if (alreadyDetectedPlugin->m_successfulLoad)
        {
            // If the plugin was successfully loaded, don't remove it from the list even if the file
            // was somehow removed.
            return false;
        }
        for (fs::path& presentDLLpath : currentlyPresentDLLsInFolder)
        {
            if (presentDLLpath == alreadyDetectedPlugin->m_filepath)
            {
                // Though this plugin isn't loaded, a record of it is already present.
                return false;
            }
        }
        // Plugin isn't loaded and though a record of it was present, the DLL is no longer available in the "plugins" folder.
        // Remove the record of it.
        return true;
    };
    dllResults.erase(std::remove_if(dllResults.begin(), dllResults.end(),
        IsPluginRecordPresentButDLLisNoLongerAvailable), dllResults.end());
    // Add records for newly detected DLLs.
    for (fs::path& presentDLLpath : currentlyPresentDLLsInFolder) {
        bool isRecordAlreadyPresentAboutThisDLL = false;
        for (std::unique_ptr<MyPluginResult>& alreadyDetectedPlugin : dllResults)
        {
            if (alreadyDetectedPlugin->m_filepath == presentDLLpath)
            {
                isRecordAlreadyPresentAboutThisDLL = true;
            }
        }
        if (isRecordAlreadyPresentAboutThisDLL)
        {
            continue;
        }
        std::unique_ptr<MyPluginResult> result = std::make_unique<MyPluginResult>(presentDLLpath);
        dllResults.push_back(std::move(result));
    }
}
void MyPluginLoader::LoadAllFoundNonloadedPlugins()
{
    LOG_DEBUG(L"[*] LoadAllFoundNonloadedPlugins():\n");
    for (std::unique_ptr<MyPluginResult>& plugin : dllResults)
    {
        if (plugin->m_successfulLoad)
        {
            continue;
        }
        LoadAndStartPlugin(*plugin);
    }
}
#include "Common_Plugins_impl/PluginLoaderSharedGlobals.h"
extern ImGuiContext* GImGui;
void MyPluginLoader::DrawPluginListControls()
{
    ImGui::Text("Found plugins: %d", dllResults.size());
    if (ImGui::Button("Update available plugin list"))
    {
        UpdateListOfAvailablePlugins();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load all nonloaded"))
    {
        LoadAllFoundNonloadedPlugins();
    }
    std::string buf;

    ImGuiShared imguiShared{ *GImGui };
    ImGui::GetAllocatorFunctions(&imguiShared.alloc_func, &imguiShared.free_func, &imguiShared.user_data);
    for (size_t i = 0; i < dllResults.size(); i++)
    {
        std::unique_ptr<MyPluginResult>& plugin = dllResults[i];
        buf = std::to_string(i + 1) + ". " + plugin->m_filepath.filename().string();
        ImGui::PushID(&plugin);
        if (plugin->m_successfulLoad)
        {
            auto DrawSuccessfullyLoadedPlugin = [&]()
            {
                auto* menuCallback = plugin->m_successfulLoad->m_pluginInterface->m_EveryFrameWhenMenuIsOpen;
                if (!menuCallback)
                {
                    ImGui::Text(buf.c_str());
                    return;
                }
                bool& isSeparateMenu = plugin->m_successfulLoad->m_isMenuOpen;
                ImGuiTreeNodeFlags treeflags = ImGuiTreeNodeFlags_FramePadding;
                bool isMightBeExpanded = true;
                if (isSeparateMenu)
                {
                    treeflags |= ImGuiTreeNodeFlags_Leaf;
                    isMightBeExpanded = false;
                }
                bool treeopen = ImGui::TreeNodeEx(
                    buf.c_str(),
                    treeflags
                );
                ImGui::SameLine();
                if (ImGui::Button("Toggle menu"))
                {
                    isSeparateMenu = !isSeparateMenu;
                }
                if (treeopen) {
                    if (isMightBeExpanded) {
                        ImGui::Separator();
                        menuCallback(imguiShared);
                        ImGui::Separator();
                    }
                    ImGui::TreePop();
                }
            };
            DrawSuccessfullyLoadedPlugin();
        }
        else
        {
            ImGui::TextDisabled(buf.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Load"))
            {
                LoadAndStartPlugin(*plugin);
            }
        }
        ImGui::PopID();
        buf.clear();
    }
}
void MyPluginLoader::DrawImGuiForPlugins_WhenMenuIsOpened()
{
    ImGuiShared imguiShared{ *GImGui };
    ImGui::GetAllocatorFunctions(&imguiShared.alloc_func, &imguiShared.free_func, &imguiShared.user_data);
    for (std::unique_ptr<MyPluginResult>& plugin : g_MyPluginLoader.dllResults)
    {
        if (!plugin->m_successfulLoad) { continue; }
        if (!plugin->m_successfulLoad->m_isMenuOpen) { continue; }
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
        if (auto* menuCallback = plugin->m_successfulLoad->m_pluginInterface->m_EveryFrameWhenMenuIsOpen) {
            if (ImGui::Begin(plugin->m_filepath.filename().string().c_str(), &plugin->m_successfulLoad->m_isMenuOpen)) {
                menuCallback(imguiShared);
            }
            ImGui::End();
        }
    }
}
void MyPluginLoader::DrawImGuiForPlugins_EvenWhenMenuIsClosed()
{
    ImGuiShared imguiShared{ *GImGui };
    ImGui::GetAllocatorFunctions(&imguiShared.alloc_func, &imguiShared.free_func, &imguiShared.user_data);
    for (const std::unique_ptr<MyPluginResult>& plugin : g_MyPluginLoader.dllResults)
    {
        if (!plugin->m_successfulLoad) { continue; }
        if (auto* everyFrameCallback = plugin->m_successfulLoad->m_pluginInterface->m_EveryFrameEvenWhenMenuIsClosed)
        {
            everyFrameCallback(imguiShared);
        }
    }
}

void MyPluginLoader::EveryFrameBeforeGraphicsUpdate()
{
    if (m_IsRequestedToUnloadPlugin)
    {
        for (std::unique_ptr<MyPluginResult>& loadedPlugin : dllResults)
        {
            if (loadedPlugin->m_successfulLoad
                && loadedPlugin->m_successfulLoad->m_isRequestedToUnload)
            {
                FreeLibrary(loadedPlugin->m_successfulLoad->m_moduleHandle);
                loadedPlugin->m_successfulLoad.reset();
            }
        }
        m_IsRequestedToUnloadPlugin = false;
    }
}
void EveryFrameBeforeGraphicsUpdate()
{
    g_MyPluginLoader.EveryFrameBeforeGraphicsUpdate();
}
void DrawPluginListControls()
{
    g_MyPluginLoader.DrawPluginListControls();
}
void DrawPluginsWhenMenuOpen()
{
    g_MyPluginLoader.DrawImGuiForPlugins_WhenMenuIsOpened();
}
void DrawPluginsEvenWhenMenuIsClosed()
{
    g_MyPluginLoader.DrawImGuiForPlugins_EvenWhenMenuIsClosed();
}

void PluginLoader_VariousHooks_Start();
void InstallCrashLog();

static void PluginLoader_MainThread(HMODULE thisDLLModule)
{
    Base::Data::thisDLLModule = thisDLLModule;
    MyLogFileLifetime _log{ AbsolutePathInMyDirectory("ACUFixes-PluginLoader.log") };
    InstallCrashLog();
    WaitUntilGameIsInitializedEnoughSoThatTheMainIntegrityCheckCanBeDisabled();
    PluginLoaderConfig::FindAndLoadConfigFileOrCreateDefault();
    DisableMainIntegrityCheck();

    PluginLoader_VariousHooks_Start();

    g_MyPluginLoader.UpdateListOfAvailablePlugins();
    g_MyPluginLoader.LoadAllFoundNonloadedPlugins();
    PresentHookOuter::BasehookSettings_PresentHookOuter basehook;
    Base::Start(basehook);
    while (!Base::Data::Detached)
    {
        // If the debugger is attached, the game can crash, though not immediately.
        // Manually repeatedly resetting the `BeingDebugged` flag is a simplistic solution,
        // but it has been enough so far.
        ClearThe_BeingDebugged_Flag();
        Sleep(100);
    }
    // The PluginLoader DLL isn't supposed to be uninjected,
    // so this code isn't supposed to be reached.
    // The comment below is from an earlier version.
    // // Helps to prevent the crash due to Present() trampoline deallocation
    // // (happens _sometimes_ with PresentHookOuter when pressing Detach in the ImGui menu)
    // // The best solution would be to not deallocate the Present() trampoline at all, like
    // // what I believe Cheat Engine's `globalalloc` does.
    Sleep(500);
}
DWORD WINAPI PluginLoader_MainThread_RAIIWrapper(LPVOID lpThreadParameter)
{
    // `FreeLibraryAndExitThread()` will prevent destructors of objects at this scope
    // from being called, that's why all work is instead done in a separate function.
    PluginLoader_MainThread((HMODULE)lpThreadParameter);
    FreeLibraryAndExitThread((HMODULE)lpThreadParameter, TRUE);
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, PluginLoader_MainThread_RAIIWrapper, hModule, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

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
        SuccessfulLoad(HMODULE moduleHandle, ACUPluginInterfaceVirtuals& pluginInterface)
            : m_moduleHandle(moduleHandle)
            , m_pluginInterface(pluginInterface)
        {}
        HMODULE m_moduleHandle;
        ACUPluginInterfaceVirtuals& m_pluginInterface;
        bool m_isRequestedToUnload = false;
        bool m_isMenuOpen = false;
    };
    std::optional<SuccessfulLoad> m_successfulLoad;
};
void PluginLoader_RequestUnloadPlugin(HMODULE dllHandle);
HMODULE PluginLoader_GetPluginIfLoaded(const wchar_t* pluginName);
using PluginLoaderResults = std::vector<std::unique_ptr<MyPluginResult>>;
class MyPluginLoader
{
public:
    MyPluginLoader();
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
#include "Common_Plugins_impl/PluginLoaderSharedGlobals.h"
#include "Common_Plugins_impl/InputHooks.h"
extern ACU::Input::InputHooks g_InputHooks;
ACUPluginLoaderSharedGlobals g_SharedVariables;
ACUPluginLoaderSharedGlobals::ACUPluginLoaderSharedGlobals()
    : m_InputHooks(g_InputHooks)
{}
MyPluginLoader::MyPluginLoader()
{
    m_PluginLoaderInterfaces.RequestUnloadPlugin = ::PluginLoader_RequestUnloadPlugin;
    m_PluginLoaderInterfaces.GetPluginIfLoaded = ::PluginLoader_GetPluginIfLoaded;
    m_PluginLoaderInterfaces.m_ImplementationSharedVariables = &g_SharedVariables;
}
void MyPluginLoader::LoadAndStartPlugin(MyPluginResult& pluginRecord)
{
    HMODULE moduleHandle = LoadLibraryW(pluginRecord.m_filepath.c_str());
    if (!moduleHandle)
    {
        return;
    }
    ACUPluginStart_fnt startFn = reinterpret_cast<ACUPluginStart_fnt>(GetProcAddress(moduleHandle, "ACUPluginStart"));
    if (!startFn)
    {
        FreeLibrary(moduleHandle);
        return;
    }
    ACUPluginInterfaceVirtuals* successfullyStartedPlugin = startFn(m_PluginLoaderInterfaces);
    if (!successfullyStartedPlugin)
    {
        FreeLibrary(moduleHandle);
        return;
    }
    pluginRecord.m_successfulLoad.emplace(moduleHandle, *successfullyStartedPlugin);
}
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
    fs::path pluginsFolder = AbsolutePathInMyDirectory(L"plugins");
    fs::create_directory(pluginsFolder);
    std::vector<fs::path> currentlyPresentDLLsInFolder;
    currentlyPresentDLLsInFolder.reserve(32);
    for (const auto& entry : std::filesystem::directory_iterator(pluginsFolder)) {
        if (entry.path().extension() == L".dll") {
            currentlyPresentDLLsInFolder.push_back(entry.path());
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
    for (std::unique_ptr<MyPluginResult>& plugin : dllResults)
    {
        if (plugin->m_successfulLoad)
        {
            continue;
        }
        LoadAndStartPlugin(*plugin);
    }
}
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
    ImGui::SameLine();
    if (ImGui::Button("Unload all"))
    {
        UnloadAllPlugins();
    }
    std::string buf;
    for (size_t i = 0; i < dllResults.size(); i++)
    {
        std::unique_ptr<MyPluginResult>& plugin = dllResults[i];
        buf = std::to_string(i + 1) + ". " + plugin->m_filepath.filename().string();
        ImGui::PushID(&plugin);
        if (plugin->m_successfulLoad)
        {
            ImGui::Text(buf.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Toggle menu"))
            {
                plugin->m_successfulLoad->m_isMenuOpen = !plugin->m_successfulLoad->m_isMenuOpen;
            }
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
extern ImGuiContext* GImGui;
void MyPluginLoader::DrawImGuiForPlugins_WhenMenuIsOpened()
{
    for (std::unique_ptr<MyPluginResult>& plugin : g_MyPluginLoader.dllResults)
    {
        if (!plugin->m_successfulLoad) { continue; }
        if (!plugin->m_successfulLoad->m_isMenuOpen) { continue; }
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
        if (ImGui::Begin(plugin->m_filepath.filename().string().c_str(), &plugin->m_successfulLoad->m_isMenuOpen))
        {
            plugin->m_successfulLoad->m_pluginInterface.EveryFrameWhenMenuIsOpen(*GImGui);
        }
        ImGui::End();
    }
}
void MyPluginLoader::DrawImGuiForPlugins_EvenWhenMenuIsClosed()
{
    for (const std::unique_ptr<MyPluginResult>& plugin : g_MyPluginLoader.dllResults)
    {
        if (!plugin->m_successfulLoad) { continue; }
        plugin->m_successfulLoad->m_pluginInterface.EveryFrameEvenWhenMenuIsClosed(*GImGui);
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
static void PluginLoader_MainThread(HMODULE thisDLLModule)
{
    Base::Data::thisDLLModule = thisDLLModule;
    MyLogFileLifetime _log{ AbsolutePathInMyDirectory("ACUFixes-PluginLoader.log") };
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

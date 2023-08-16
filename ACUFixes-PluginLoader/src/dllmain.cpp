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
using ACUPluginStart_fnt = ACUPluginInterfaceVirtuals*(*)();
struct MyPluginResult
{
    fs::path m_filepath;
    struct SuccessfulLoad
    {
        HMODULE m_moduleHandle;
        ACUPluginStart_fnt m_startFn;
        ACUPluginInterfaceVirtuals* m_pluginInterface = nullptr;
    };
    std::optional<SuccessfulLoad> m_successfulLoad;
};
using PluginLoaderResults = std::vector<MyPluginResult>;
MyPluginResult LoadPlugin(const fs::path& filepath)
{
    MyPluginResult result;
    result.m_filepath = filepath;
    HMODULE moduleHandle = LoadLibraryW(result.m_filepath.c_str());
    if (!moduleHandle)
    {
        return result;
    }
    ACUPluginStart_fnt startFn = reinterpret_cast<ACUPluginStart_fnt>(GetProcAddress(moduleHandle, "ACUPluginStart"));
    if (!startFn)
    {
        FreeLibrary(moduleHandle);
        return result;
    }
    result.m_successfulLoad = { moduleHandle, startFn };
    return result;
}
class MyPluginLoader
{
public:
    void LoadAllPluginsInFolder();
    void UnloadAllPlugins();
    void DrawImGuiControls();
    void DrawImGuiForPlugins_WhenMenuIsOpened();
    void DrawImGuiForPlugins_EvenWhenMenuIsClosed();
    ~MyPluginLoader() { UnloadAllPlugins(); }
private:
    PluginLoaderResults dllResults;
} g_MyPluginLoader;
void MyPluginLoader::UnloadAllPlugins()
{
    for (MyPluginResult& dll : dllResults)
    {
        if (dll.m_successfulLoad)
        {
            FreeLibrary(dll.m_successfulLoad->m_moduleHandle);
        }
    }
    dllResults.clear();
}
void MyPluginLoader::LoadAllPluginsInFolder()
{
    UnloadAllPlugins();
    fs::path pluginsFolder = AbsolutePathInMyDirectory(L"plugins");

    fs::create_directory(pluginsFolder);
    for (const auto& entry : std::filesystem::directory_iterator(pluginsFolder)) {
        if (entry.path().extension() == L".dll") {
            MyPluginResult result = LoadPlugin(entry.path());
            dllResults.push_back(std::move(result));
        }
    }

    for (MyPluginResult& dll : dllResults)
    {
        if (dll.m_successfulLoad)
        {
            dll.m_successfulLoad->m_pluginInterface = dll.m_successfulLoad->m_startFn();
            dll.m_successfulLoad->m_pluginInterface->m_imguiCtxPtr = GImGui;
        }
    }
}
void MyPluginLoader::DrawImGuiControls()
{
    ImGui::Text("Found plugins: %d", dllResults.size());
    if (ImGui::Button("Load"))
    {
        LoadAllPluginsInFolder();
    }
    if (ImGui::Button("Unload"))
    {
        UnloadAllPlugins();
    }
}
void MyPluginLoader::DrawImGuiForPlugins_WhenMenuIsOpened()
{
    for (const MyPluginResult& plugin : g_MyPluginLoader.dllResults)
    {
        if (!plugin.m_successfulLoad) { continue; }
        plugin.m_successfulLoad->m_pluginInterface->ImGui_WhenMenuIsOpen();
    }
}
void MyPluginLoader::DrawImGuiForPlugins_EvenWhenMenuIsClosed()
{
    for (const MyPluginResult& plugin : g_MyPluginLoader.dllResults)
    {
        if (!plugin.m_successfulLoad) { continue; }
        plugin.m_successfulLoad->m_pluginInterface->ImGui_EvenWhenMenuIsClosed();
    }
}

void DrawPluginLoaderControls()
{
    g_MyPluginLoader.DrawImGuiControls();
}
void DrawPluginsWhenMenuOpen()
{
    g_MyPluginLoader.DrawImGuiForPlugins_WhenMenuIsOpened();
}
void DrawPluginsEvenWhenMenuIsClosed()
{
    g_MyPluginLoader.DrawImGuiForPlugins_EvenWhenMenuIsClosed();
}

static void PluginLoader_MainThread(HMODULE thisDLLModule)
{
    Base::Data::thisDLLModule = thisDLLModule;
    MyLogFileLifetime _log{ AbsolutePathInMyDirectory("ACUFixes-PluginLoader.log") };
    WaitUntilGameIsInitializedEnoughSoThatTheMainIntegrityCheckCanBeDisabled();
    PluginLoaderConfig::FindAndLoadConfigFileOrCreateDefault();
    DisableMainIntegrityCheck();
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

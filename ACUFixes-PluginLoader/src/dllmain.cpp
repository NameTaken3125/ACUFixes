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


void PluginLoader_VariousHooks_Start();
void PluginLoader_FindAndLoadPlugins();
void InstallCrashLog();
void SuspendThreadsUntilCheatEnginesVEHDebuggerIsAttached();


static void PluginLoader_MainThread(HMODULE thisDLLModule)
{
    //SuspendThreadsUntilCheatEnginesVEHDebuggerIsAttached();
    Base::Data::thisDLLModule = thisDLLModule;
    MyLogFileLifetime _log{ AbsolutePathInMyDirectory(THIS_DLL_PROJECT_NAME ".log") };
    InstallCrashLog();
    WaitUntilGameIsInitializedEnoughSoThatTheMainIntegrityCheckCanBeDisabled();
    PluginLoaderConfig::FindAndLoadConfigFileOrCreateDefault();
    DisableMainIntegrityCheck();

    PluginLoader_VariousHooks_Start();
    PluginLoader_FindAndLoadPlugins();

    PresentHookOuter::BasehookSettings_PresentHookOuter basehook;
    Base::Fonts::SetFontSize(g_PluginLoaderConfig.fontSize);
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

#include "pch.h"

#include "base.h"
#include "MyLog.h"
#include "PluginLoaderConfig.h"
#include "PresentHookOuter.h"
#include "EarlyHooks/ThreadOperations.h"

#define LOG_FILENAME THIS_DLL_PROJECT_NAME ".log"


fs::path AbsolutePathInMyDirectory(const fs::path& filenameRel);
void PluginLoader_VariousHooks_Start();
void PluginLoader_FindAndLoadPlugins();
void InstallCrashLog();
void SuspendThreadsUntilCheatEnginesVEHDebuggerIsAttached();
void ClearThe_BeingDebugged_Flag();


std::optional<MyLogFileLifetime> g_LogLifetime;
DWORD WINAPI PluginLoaderSetup_AfterMICIsDisabled(LPVOID lpThreadParameter)
{
    HANDLE evPluginLoaderReady = (HANDLE)lpThreadParameter;
    PluginLoader_VariousHooks_Start();
    PluginLoader_FindAndLoadPlugins();

    PresentHookOuter::BasehookSettings_PresentHookOuter basehook;
    Base::Fonts::SetFontSize(g_PluginLoaderConfig.fontSize);
    Base::Start(basehook);
    SetEvent(evPluginLoaderReady);
    while (!Base::Data::Detached)
    {
        // If the debugger is attached, the game can crash, though not immediately.
        // Repeatedly resetting the `BeingDebugged` flag on a timer is a simplistic solution,
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
    return TRUE;
}
void MainIntegrityCheckHasJustBeenDisabled()
{
    HANDLE evPluginLoaderReady = CreateEventA(0, 0, 0, 0);
    CreateThread(nullptr, 0, PluginLoaderSetup_AfterMICIsDisabled, evPluginLoaderReady, 0, nullptr);
    if (evPluginLoaderReady)
        WaitForSingleObject(evPluginLoaderReady, INFINITE);
}
#include "ACU/Threads/KnownThreads.h"
void BeforeGameMainThreadStarted_HookTheStartAddress();
BOOL DllMainOnProcessAttach(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    Base::Data::thisDLLModule = hModule;
    g_LogLifetime.emplace(AbsolutePathInMyDirectory(LOG_FILENAME));
    PluginLoaderConfig::FindAndLoadConfigFileOrCreateDefault();
    const bool isInMainThread = GetThreadStartAddress(GetCurrentThread()) == ACU::Threads::ThreadStartAddr_MainThread;
    if (!isInMainThread)
    {
        LOG_DEBUG(DefaultLogger,
            "[EarlyHooks]\n"
            "   Too late to install the early hooks.\n"
            "   This version of ACUFixes is supposed to be installed in the following way:\n"
            "   Place the files directly into the \"Assassin's Creed Unity\" folder (where the \"ACU.exe\" file is).\n"
            "   Game's folder structure will look like this:\n"
            "       Assassin's Creed Unity/\n"
            "       +-- ACUFixes/\n"
            "       |   +-- plugins/\n"
            "       |   |   +--ACUFixes.dll\n"
            "       |   +-- ACUFixes-PluginLoader.dll\n"
            "       +-- ACU.exe\n"
            "       +-- version.dll\n"
        );
        const char* msg =
            "[" THIS_DLL_PROJECT_NAME "]:\n"
            "Too late to install the early hooks.\n"
            "The mod has probably been installed incorrectly.\n"
            "The mod will not start.\n"
            "See \"" LOG_FILENAME "\" log file for more details.\n"
            ;
        MessageBoxA(NULL, msg, THIS_DLL_PROJECT_NAME, MB_OK | MB_ICONWARNING);
        return FALSE;
    }
    BeforeGameMainThreadStarted_HookTheStartAddress();
    return TRUE;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        return DllMainOnProcessAttach(hModule, dwReason, lpReserved);
        break;
    }
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

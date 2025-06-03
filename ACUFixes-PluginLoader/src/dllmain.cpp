#include "pch.h"

#include "base.h"
#include "MyLog.h"
#include "PluginLoaderConfig.h"
#include "PresentHookOuter.h"
#include "EarlyHooks/ThreadOperations.h"

#define LOG_FILENAME THIS_DLL_PROJECT_NAME ".log"


fs::path AbsolutePathInMyDirectory(const fs::path& filenameRel);
void InstallCrashLog();
std::optional<MyLogFileLifetime> g_LogLifetime;
void ClearThe_BeingDebugged_Flag();

void PluginLoader_VariousHooks_Start();
void PluginLoader_FirstTimeGatherPluginsAndCheckCompatibility();
void PluginLoader_WhenSafeToApplyCodePatches();

bool g_IsReadyToStart_PluginsInitialLoadAndVersionCheck = false;
bool g_IsFinished_PluginsInitialLoadAndVersionCheck = false;
bool g_IsMICDisabled = false;
void WaitUntilMICIsDisabled();
bool g_IsPluginLoaderFullyInitialized = false;

void StartupTasksAndHeartbeat()
{
    while (!g_IsReadyToStart_PluginsInitialLoadAndVersionCheck)
        Sleep(500);
    PluginLoader_FirstTimeGatherPluginsAndCheckCompatibility();
    g_IsFinished_PluginsInitialLoadAndVersionCheck = true;
    WaitUntilMICIsDisabled();
    PluginLoader_VariousHooks_Start();
    PluginLoader_WhenSafeToApplyCodePatches();

    PresentHookOuter::BasehookSettings_PresentHookOuter basehook;
    Base::Fonts::SetFontSize(g_PluginLoaderConfig.fontSize);
    Base::Start(basehook);
    g_IsPluginLoaderFullyInitialized = true;
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
    return;
}
template<typename Callable>
void WaitAndShowMessageBoxesIfTakingTooLong(const std::string_view& taskDescription, Callable&& checkIfFinished)
{
    int secsBetweenNotifications = 20;
    ULONGLONG tsStartedToLoadPlugins = GetTickCount64();
    ULONGLONG tsLastNotification = tsStartedToLoadPlugins;
    while (true)
    {
        if (checkIfFinished())
        {
            break;
        }
        ULONGLONG now_ = GetTickCount64();
        const ULONGLONG millisElapsedSinceLastNotification = now_ - tsLastNotification;
        const ULONGLONG millisUntilNotification = secsBetweenNotifications * 1000;
        if (millisElapsedSinceLastNotification > millisUntilNotification)
        {
            ImGuiTextBuffer buf;
            buf.appendf(
                "----------------------------------------------------\n"
                "%s\n"
                "----------------------------------------------------\n"
                , taskDescription.data()
            );
            buf.appendf(
                "It's been %.0f seconds since the wait started.\n"
                "It's taking an unusually long time.\n"
                "Check the \"" LOG_FILENAME "\" for errors if this is not normal.\n"
                "If this keeps happening, try removing some of the plugins.\n"
                "This notification is shown every %u seconds.\n"
                "Continue to wait? Press \"No\" to exit the process.\n"
                , ((now_ - tsStartedToLoadPlugins)) / 1000.0f
                , secsBetweenNotifications
            );
            int mbResult = MessageBoxA(NULL, buf.c_str(), THIS_DLL_PROJECT_NAME,
                MB_ICONWARNING |
                MB_YESNO |
                MB_SETFOREGROUND);
            if (mbResult == IDNO)
            {
                const int exitCode = 'PLTL'; // ==0x504c544c == "Plugin Loader Too Long"
                std::exit(exitCode);
            }
            now_ = GetTickCount64();
            tsLastNotification = now_;
        }
        Sleep(500);
    }
}
void WaitForInitialLoadingOfPluginsToFinish()
{
    WaitAndShowMessageBoxesIfTakingTooLong("Waiting to finish plugins' initial loading and version check.", []() { return g_IsFinished_PluginsInitialLoadAndVersionCheck; });
}
void WaitForFinishedPluginInit()
{
    WaitAndShowMessageBoxesIfTakingTooLong("Waiting for the plugins to finish loading.", []() { return g_IsPluginLoaderFullyInitialized; });
}
DWORD WINAPI PluginLoaderThread_StartupTasksAndHeartbeat(LPVOID lpThreadParameter)
{
    StartupTasksAndHeartbeat();
    FreeLibraryAndExitThread((HMODULE)lpThreadParameter, TRUE);
    return TRUE;
}
void WhenReadyForInitialLoadingOfPlugins()
{
    g_IsReadyToStart_PluginsInitialLoadAndVersionCheck = true;
}
#include "ACU/Threads/KnownThreads.h"
void BeforeGameMainThreadStarted_HookTheStartAddress();
bool IsCompatibleGameVersion()
{
    // I check some bytes at the start of the Main Thread's code.
    // I don't have other game versions, but I trust their entry points ought to be different.
    void* mainThreadStart_Addr = (void*)ACU::Threads::ThreadStartAddr_MainThread;
    unsigned char mainThreadStart_Bytes[] = {
        0xE9, 0x0B, 0x25, 0x00, 0x00, 0xE9, 0x34, 0x37,
        0xDF, 0xFE, 0xE9, 0x0A, 0x87, 0xC4, 0xFE, 0x69,
        0xD2, 0x0A, 0x00, 0x00, 0x00, 0x66, 0x85, 0xE9,
        0xF6, 0xC1, 0x03, 0x01, 0xC2, 0xE9, 0x96, 0x74
    };
    constexpr std::size_t numBytesToRead = std::size(mainThreadStart_Bytes);
    std::array<char, numBytesToRead> buf{ 0 };
    SIZE_T numBytesRead{};
    if (!ReadProcessMemory(GetCurrentProcess(), (void*)mainThreadStart_Addr, &buf[0], numBytesToRead, &numBytesRead)) return false;
    return (std::array<char, numBytesToRead>&)mainThreadStart_Bytes == buf;
}
BOOL DllMainOnProcessAttach(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (!IsCompatibleGameVersion())
    {
        MessageBoxA(NULL,
            "Game version is incompatible.\n"
            "Assassin's Creed Unity Version 1.5.0 is required.\n"
            "The mod will not start.\n"
            , THIS_DLL_PROJECT_NAME,
            MB_OK |
            MB_ICONERROR |
            MB_SETFOREGROUND);
        return FALSE;
    }
    Base::Data::thisDLLModule = hModule;
    g_LogLifetime.emplace(AbsolutePathInMyDirectory(LOG_FILENAME));
    PluginLoaderConfig::FindAndLoadConfigFileOrCreateDefault();
    InstallCrashLog();


    CreateThread(nullptr, 0, PluginLoaderThread_StartupTasksAndHeartbeat, hModule, 0, nullptr);


    const bool isInMainThread = GetThreadStartAddress(GetCurrentThread()) == ACU::Threads::ThreadStartAddr_MainThread;
    const bool isEarlyHooksPossible = isInMainThread;
    if (isEarlyHooksPossible)
    {
        // Do multistage plugin initialization, with possible early hooks
        // between the point the main thread starts and the point the main integrity check is disabled.
        BeforeGameMainThreadStarted_HookTheStartAddress();
    }
    else
    {
        // Too late to install early hooks, don't bother trying.
        // Just load the plugins without blocking the main thread.
        WhenReadyForInitialLoadingOfPlugins();
    }
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

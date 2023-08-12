/*
main.cpp
Brings the app's components together.
*/

#include "pch.h"

#include "base.h"
#include "MyVariousHacks.h"
#include "MyLog.h"
#include "MainConfig.h"

#include "ACU/ACUGetSingletons.h"

// Required so that the game doesn't crash on code modification.
void DisableMainIntegrityCheck();
void WaitUntilGameIsInitializedEnoughSoThatTheMainIntegrityCheckCanBeDisabled();
void ClearThe_BeingDebugged_Flag();
// Respond to input: toggle hacks, etc.
LRESULT CALLBACK WndProc_HackControls(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Just re-declaring these as a reminder that they're the user-defined parts
// that are needed to make the BaseHook work.
void Base::ImGuiLayer_EvenWhenMenuIsClosed();
void Base::ImGuiLayer_WhenMenuIsOpen();
fs::path AbsolutePathInMyDirectory(const fs::path& filenameRel);

extern "C" __declspec(dllexport) void ACUPluginStart()
{
	MyLogFileLifetime _log{ AbsolutePathInMyDirectory("acufixes-log.log") };
	MainConfig::FindAndLoadConfigFileOrCreateDefault();
	MyVariousHacks::Start();
}
extern "C" __declspec(dllexport) void ACUPluginStop()
{

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        Base::Data::thisDLLModule = hModule;
        DisableThreadLibraryCalls(hModule);
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    default:
        break;
    }
    return TRUE;
}
/*
main.cpp
Brings the app's components together.
*/

#include "pch.h"

#include "base.h"
#include "PresentHookOuter.h"
#include "MyVariousHacks.h"
#include "MyLog.h"
#include "MainConfig.h"

#include "ACU/ACUGetSingletons.h"

// Required so that the game doesn't crash on code modification.
void DisableMainIntegrityCheck();
void WaitUntilGameIsInitializedEnoughSoThatTheMainIntegrityCheckCanBeDisabled();
// Respond to input: toggle hacks, etc.
LRESULT CALLBACK WndProc_HackControls(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Just re-declaring these as a reminder that they're the user-defined parts
// that are needed to make the BaseHook work.
void Base::ImGuiLayer_EvenWhenMenuIsClosed();
void Base::ImGuiLayer_WhenMenuIsOpen();
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
static void MainThread(HMODULE thisDLLModule)
{
    Base::Data::thisDLLModule = thisDLLModule;
    MyLogFileLifetime _log{ AbsolutePathInMyDirectory("acufixes-log.log") };
    WaitUntilGameIsInitializedEnoughSoThatTheMainIntegrityCheckCanBeDisabled();
    MainConfig::FindAndLoadConfigFileOrCreateDefault();
    DisableMainIntegrityCheck();
    std::unique_ptr<Base::Settings> basehook;
    if (g_Config.imgui_useImGui)
    {
        basehook = std::make_unique<PresentHookOuter::BasehookSettings_PresentHookOuter>();
    }
    else
    {
        basehook = std::make_unique<Base::BasehookSettings_OnlyWNDPROC>((HWND)ACU::GetWindowHandle(), WndProc_HackControls);
    }
    // I've put MyVariousHacks::Start() _before_ Base::Start() because the opposite order
    // for some reason produces a small but noticeable stutter on injection.
    MyVariousHacks::Start();
    Base::Start(*basehook);
    while (!Base::Data::Detached)
    {
        Sleep(100);
    }

    // Helps to prevent the crash due to Present() trampoline deallocation
    // (happens _sometimes_ with PresentHookOuter when pressing Detach in the ImGui menu)
    // The best solution would be to not deallocate the Present() trampoline at all, like
    // what I believe Cheat Engine's `globalalloc` does.
    Sleep(500);
}
DWORD WINAPI MainThreadRAIIWrapper(LPVOID lpThreadParameter)
{
    // `FreeLibraryAndExitThread()` will prevent destructors of objects at this scope
    // from being called, that's why all work is instead done in a separate function.
    MainThread((HMODULE)lpThreadParameter);
    FreeLibraryAndExitThread((HMODULE)lpThreadParameter, TRUE);
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, MainThreadRAIIWrapper, hModule, 0, nullptr);
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
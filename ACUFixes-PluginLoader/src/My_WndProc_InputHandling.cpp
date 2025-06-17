#include "pch.h"

#include "base.h"
#include "MyLog.h"
#include "PluginLoaderConfig.h"

#include "ImGuiConsole.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_HackControls(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool dontForwardToGame = false;

    // In Assassin's Creed Unity, when going Fullscreen (but not Borderless Fullscreen)
    // this fails to receive WM_MOUSEWHEEL and mouse button events
    // such as WM_LBUTTONDOWN.
    // WM_MOUSEMOVE is ok, go figure.

    // https://stackoverflow.com/questions/7252398/convert-windows-message-ids-to-text
    //if (uMsg != WM_NCHITTEST
    //    && uMsg != WM_MOUSEMOVE
    //    && uMsg != WM_SETCURSOR
    //    )
    //{
    //    LOG_DEBUG(DefaultLogger, "[WndProc]Msg: %u, wParam: %llx, lParam: %llX\n", uMsg, wParam, lParam);
    //}
    constexpr unsigned __int64 isPreviouslyPressedMask = 1 << 30;
    if (uMsg == WM_KEYDOWN)
    {
        const int keyCode = (int)wParam;
        const bool isPreviouslyPressed = lParam & isPreviouslyPressedMask;
        const bool isKeyJustPressed = !isPreviouslyPressed;
        if (isKeyJustPressed)
        {
            if (keyCode == Base::Data::Keys::ToggleMenu
                || keyCode == (UINT)g_PluginLoaderConfig.hotkey_ToggleMenu.get())
            {
                if (Base::Data::IsImGuiInitialized)
                {
                    Base::Data::ShowMenu = !Base::Data::ShowMenu;
                }
            }
            else if (keyCode == (UINT)g_PluginLoaderConfig.hotkey_ToggleConsole.get())
            {
                ToggleConsoleMode();
            }
            else if (
                g_PluginLoaderConfig.developerOptions->isActive
                && g_PluginLoaderConfig.developerOptions->canUninjectPluginLoader->isActive
                && keyCode == (UINT)g_PluginLoaderConfig.developerOptions->canUninjectPluginLoader->hotkey_UninjectPluginLoader.get()
                )
            {
                Base::Detach();
            }
        }
    }
    LRESULT result{};
    if (Base::Data::IsImGuiInitialized)
        result = ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    if (g_PluginLoaderConfig.developerOptions->continueLoadingGameWhileItIsNotFocused)
        dontForwardToGame =
            uMsg == WM_KILLFOCUS
            || (uMsg == WM_ACTIVATE && wParam == 0)
            || (uMsg == WM_ACTIVATEAPP && wParam == FALSE)
            ;
    if (!dontForwardToGame)
        result = CallWindowProc(Base::Data::oWndProc, hWnd, uMsg, wParam, lParam);
    return result;
}

#include "pch.h"

#include "base.h"
#include "MyLog.h"
#include "PluginLoaderConfig.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_HackControls(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // In Assassin's Creed Unity, when going Fullscreen (but not Borderless Fullscreen)
    // this fails to receive WM_MOUSEWHEEL and mouse button events
    // such as WM_LBUTTONDOWN.
    // WM_MOUSEMOVE is ok, go figure.
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
                    ImGui::GetIO().MouseDrawCursor = Base::Data::ShowMenu;
                }
            }
            else if (keyCode == Base::Data::Keys::DetachDll
                || keyCode == (UINT)g_PluginLoaderConfig.hotkey_UnloadMod.get())
            {
                Base::Detach();
            }
        }
    }
    if (Base::Data::IsImGuiInitialized)
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    return CallWindowProc(Base::Data::oWndProc, hWnd, uMsg, wParam, lParam);
}

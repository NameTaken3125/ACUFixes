#include "pch.h"

#include "base.h"
#include "MyVariousHacks.h"
#include "MyLog.h"

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
            switch (keyCode)
            {
            case Base::Data::Keys::ToggleMenu:
            {
                if (Base::Data::IsImGuiInitialized)
                {
                    Base::Data::ShowMenu = !Base::Data::ShowMenu;
                    ImGui::GetIO().MouseDrawCursor = Base::Data::ShowMenu;
                }
                break;
            }
            case Base::Data::Keys::DetachDll:
                Base::Detach();
                break;
            }
            MyVariousHacks::MyHacks_OnKeyJustPressed(keyCode);
        }
    }
    if (Base::Data::IsImGuiInitialized)
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    return CallWindowProc(Base::Data::oWndProc, hWnd, uMsg, wParam, lParam);
}

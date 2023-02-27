#include "pch.h"

#include "base.h"
#include "MyVariousHacks.h"
#include "MyLog.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_HackControls(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    constexpr unsigned __int64 isPreviouslyPressedMask = 1 << 30;
    if (uMsg == WM_KEYDOWN)
    {
        const int keyCode = (int)wParam;
        const bool isPreviouslyPressed = lParam & isPreviouslyPressedMask;
        const bool isKeyJustPressed = !isPreviouslyPressed;
        if (isKeyJustPressed)
        {
            LOG_DEBUG("Just pressed: %ld\n", keyCode);
            switch (keyCode)
            {
            case Base::Data::Keys::ToggleMenu:
            {
                if (Base::Data::IsImGuiInitialized)
                {
                    Base::Data::ShowMenu = !Base::Data::ShowMenu;
                    ImGui::GetIO().MouseDrawCursor = Base::Data::ShowMenu;
                    break;
                }
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

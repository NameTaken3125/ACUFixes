#include "pch.h"

#include "ACU/InputContainer.h"
#include "ACU/Enum_MouseButtons.h"

#include "ACU_InputUtils.h"
#include "ACU_DefineNativeFunction.h"



DEFINE_GAME_FUNCTION(InputContainer__IsLongPressJustHappened, 0x1427202B0, char, __fastcall, (InputContainer* a1, ActionKeyCode p_actionKeyCode, float p_testHowLong, __int64 a4));
DEFINE_GAME_FUNCTION(InputContainer__IsJustShortPressed, 0x142720530, bool, __fastcall, (InputContainer* a1, ActionKeyCode a2, float p_noLongerThanThis, __int64 p_is0forShortPressCheck));

#include "base.h"
void UpdateImGuiMouseInput(MouseState& mouseState)
{
    // When going Fullscreen (but not Borderless Fullscreen)
    // the `ImGui_ImplWin32_WndProcHandler()` function doesn't receive most mouse messages:
    // WM_MOUSEWHEEL and mouse button events such as WM_LBUTTONDOWN.
    // In fact, when in Fullscreen, the WndProc of the main window doesn't receive
    // these messages either.
    // WM_MOUSEMOVE is ok, go figure.
    // This means that the ImGui menu reacts to mouse move (buttons are highlighted on hover,
    // for example), but not to clicks, drags etc.
    // Somehow (by using an additional WndProc in some hidden window?), the game is still aware
    // of the mouse button updates and I use the data available in the Raw Input Hook
    // to update the mouse state variables in `ImGuiIO`.
    if (ImGui::GetCurrentContext() == NULL)
        return;

    ImGuiIO& io = ImGui::GetIO();
    HWND hwnd = Base::Data::hWindow;

    for (uint8 button = 0; button < std::size(mouseState.mouseButtonStates); button++)
    {
        uint8 buttonState = mouseState.mouseButtonStates[button];
        const bool isMouseButtonUp = buttonState == 0;
        if (isMouseButtonUp)
        {
            io.MouseDown[button] = false;
            if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
                ::ReleaseCapture();
            continue;
        }
        const bool isMouseButtonDown = buttonState == 0x80;
        if (isMouseButtonDown)
        {
            if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
                ::SetCapture(hwnd);
            io.MouseDown[button] = true;
        }
    }

    // `io.MouseWheel` is accumulated. This is slightly (for now) problematic
    // because in Borderless Fullscreen (when WM_MOUSEWHEEL _is_ received)
    // it gets accumulated twice: once here, once in ImGui_ImplWin32_WndProcHandler.
    // Better than not working in Fullscreen at all, I guess.
    io.MouseWheel += (float)(mouseState.mouseWheelDeltaInt_usedInMenus) / (float)WHEEL_DELTA;
}
void GameRawInputHook_ImGuiLayer(InputContainerBig& inpCont)
{
    if (!Base::Data::IsImGuiInitialized)
    {
        return;
    }
    UpdateImGuiMouseInput(inpCont.mouseState);
    if (Base::Data::ShowMenu)
    {
        std::memset(inpCont.isPressed_byScancode, 0, sizeof(inpCont.isPressed_byScancode));
        inpCont.mouseState.mouseDeltaIntForCamera_X = inpCont.mouseState.mouseDeltaIntForCamera_Y = 0;
        inpCont.mouseState.mouseDeltaInt_X = inpCont.mouseState.mouseDeltaInt_Y = 0;
        inpCont.mouseState.mouseWheelDeltaInt = inpCont.mouseState.mouseWheelDeltaInt_usedInMenus = 0;
        std::memset(inpCont.mouseState.mouseButtonStates, 0, sizeof(inpCont.mouseState.mouseButtonStates));
    }
}

namespace ACU::Input {

class InputHooks
{
    std::array<uint8, 256> m_PrevFrameKeyStates;
    std::array<uint8, 256> m_ThisFrameKeyStates;
    std::array<uint8, 5> m_PrevFrameMouseButtonStates;
    std::array<uint8, 5> m_ThisFrameMouseButtonStates;

    void UpdateEvolvedKeyStates(InputContainerBig& inpCont)
    {
        constexpr size_t keyStatesArraySize = sizeof(m_PrevFrameKeyStates);
        static_assert(keyStatesArraySize == sizeof(m_ThisFrameKeyStates));
        static_assert(keyStatesArraySize == sizeof(InputContainerBig::isPressed_byScancode));
        constexpr size_t mouseButtonStatesArraySize = sizeof(m_PrevFrameMouseButtonStates);
        static_assert(mouseButtonStatesArraySize == sizeof(m_ThisFrameMouseButtonStates));
        static_assert(mouseButtonStatesArraySize == sizeof(MouseState::mouseButtonStates));
        std::memcpy(&m_PrevFrameKeyStates, &m_ThisFrameKeyStates, keyStatesArraySize);
        std::memcpy(&m_ThisFrameKeyStates, &inpCont.isPressed_byScancode, keyStatesArraySize);
        std::memcpy(&m_PrevFrameMouseButtonStates, &m_ThisFrameMouseButtonStates, mouseButtonStatesArraySize);
        std::memcpy(&m_ThisFrameMouseButtonStates, &inpCont.mouseState.mouseButtonStates, mouseButtonStatesArraySize);
    }
public:
    void OnRawInputUpdate(InputContainerBig& inpCont)
    {
        UpdateEvolvedKeyStates(inpCont);
        GameRawInputHook_ImGuiLayer(inpCont);
    }
    bool IsJustPressed(uint8 scancode)
    {
        return m_ThisFrameKeyStates[scancode] && !m_PrevFrameKeyStates[scancode];
    }
    bool IsJustPressed(MouseButton mouseButton)
    {
        return m_ThisFrameMouseButtonStates[(int)mouseButton] && !m_PrevFrameMouseButtonStates[(int)mouseButton];
    }
    bool IsJustReleased(MouseButton mouseButton)
    {
        return !m_ThisFrameMouseButtonStates[(int)mouseButton] && m_PrevFrameMouseButtonStates[(int)mouseButton];
    }
    bool IsJustReleased(BindableKeyCode_Keyboard keycode)
    {
        return !m_ThisFrameKeyStates[(int)keycode] && m_PrevFrameKeyStates[(int)keycode];
    }
    static InputHooks& GetSingleton()
    {
        static InputHooks singleton;
        return singleton;
    }
};

InputContainerBig* Get_InputContainerBig()
{
    return HasInputContainers::GetSingleton()->p_10->inputContainerBig;
}
bool IsPressed(BindableKeyCode_Keyboard keycode)
{
    return ACU::Input::Get_InputContainerBig()->isPressed_byScancode[(uint32)keycode];
}
bool IsPressed(MouseButton mouseBtn)
{
    return ACU::Input::Get_InputContainerBig()->mouseState.mouseButtonStates[(uint32)mouseBtn];
}
bool IsPressed(ActionKeyCode actionKey)
{
    return InputContainer::GetMainSingleton().keyStates_thisFrame.IsPressed(actionKey);
}
bool IsJustPressed(BindableKeyCode_Keyboard keycode)
{
    return InputHooks::GetSingleton().IsJustPressed((uint8)keycode);
}
bool IsJustPressed(MouseButton mouseButton)
{
    return InputHooks::GetSingleton().IsJustPressed(mouseButton);
}
bool IsJustPressed(ActionKeyCode actionKey)
{
    auto& inpCont = InputContainer::GetMainSingleton();
    return inpCont.keyStates_thisFrame.IsPressed(actionKey)
        && !inpCont.keyStates_prevFrame.IsPressed(actionKey);
}
bool IsJustReleased(MouseButton mouseButton)
{
    return InputHooks::GetSingleton().IsJustReleased(mouseButton);
}
bool IsJustReleased(BindableKeyCode_Keyboard keycode)
{
    return InputHooks::GetSingleton().IsJustReleased(keycode);
}
bool IsJustPressedLong(ActionKeyCode actionKey, float howLong)
{
    return InputContainer__IsLongPressJustHappened(&InputContainer::GetMainSingleton(), actionKey, howLong, 0);
}
bool IsJustPressedShort(ActionKeyCode actionKey, float noLongerThanThis)
{
    return InputContainer__IsJustShortPressed(&InputContainer::GetMainSingleton(), actionKey, noLongerThanThis, 0);
}


bool IsPressedRMB() { return InputContainer::GetMainSingleton().keyStates_thisFrame.IsPressed(ActionKeyCode::RMB); }

} // namespace ACU::Input

#include "Hack_GameRawInputHook.h"

void BeforeActionKeysAreUpdatedFromScancodes_RunRawInputHooks(AllRegisters* params)
{
    InputContainerBig& inpBig = *(InputContainerBig*)params->rbx_;
    ACU::Input::InputHooks::GetSingleton().OnRawInputUpdate(inpBig);
}
GameRawInputHook::GameRawInputHook()
{
    uintptr_t beforeActionKeysAreUpdatedFromScancodes = 0x14273BC64;
    PresetScript_CCodeInTheMiddle(
        beforeActionKeysAreUpdatedFromScancodes, 6,
        BeforeActionKeysAreUpdatedFromScancodes_RunRawInputHooks, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

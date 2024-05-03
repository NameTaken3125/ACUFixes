#include "pch.h"

#include "ACU/InputContainer.h"
#include "ACU/Enum_MouseButtons.h"

#include "Common_Plugins/ACU_InputUtils.h"
#include "Common_Plugins_impl/InputHooks.h"
#include "ACU_DefineNativeFunction.h"



DEFINE_GAME_FUNCTION(InputContainer__IsLongPressJustHappened, 0x1427202B0, char, __fastcall, (InputContainer* a1, ActionKeyCode p_actionKeyCode, float p_testHowLong, __int64 a4));
DEFINE_GAME_FUNCTION(InputContainer__IsJustShortPressed, 0x142720530, bool, __fastcall, (InputContainer* a1, ActionKeyCode a2, float p_noLongerThanThis, __int64 p_is0forShortPressCheck));


namespace ACU::Input {

InputContainerBig* Get_InputContainerBig()
{
    return HasInputContainers::GetSingleton()->p_10->inputContainerBig;
}
bool IsPressed(BindableKeyCode_Keyboard keycode)
{
    return ACU::Input::Get_InputContainerBig()->isPressed_byScancode[(uint32)keycode];
}
bool IsPressed(BindableKeyCode keycode)
{
    constexpr uint16 keyboardRange_start = 1;
    constexpr uint16 keyboardRange_end = 255;
    constexpr uint16 mouseRange_start = (uint16)BindableKeyCode::MOUSE_LMB;
    constexpr uint16 mouseRange_end = (uint16)BindableKeyCode::MOUSE_X5;
    const uint16 keycodeInt = (uint16)keycode;
    if (keycodeInt >= mouseRange_start && keycodeInt <= mouseRange_end)
    {
        const uint8 mouseKeyCode = keycodeInt - mouseRange_start;
        return IsPressed((MouseButton)mouseKeyCode);
    }
    if (keycodeInt >= keyboardRange_start && keycodeInt <= keyboardRange_end)
    {
        const uint8 keyboardKeycode = keycodeInt;
        return IsPressed((BindableKeyCode_Keyboard)keyboardKeycode);
    }
    return false;
}
bool IsJustPressed(BindableKeyCode keycode)
{
    constexpr uint16 keyboardRange_start = 1;
    constexpr uint16 keyboardRange_end = 255;
    constexpr uint16 mouseRange_start = (uint16)BindableKeyCode::MOUSE_LMB;
    constexpr uint16 mouseRange_end = (uint16)BindableKeyCode::MOUSE_X5;
    const uint16 keycodeInt = (uint16)keycode;
    if (keycodeInt >= mouseRange_start && keycodeInt <= mouseRange_end)
    {
        const uint8 mouseKeyCode = keycodeInt - mouseRange_start;
        return IsJustPressed((MouseButton)mouseKeyCode);
    }
    if (keycodeInt >= keyboardRange_start && keycodeInt <= keyboardRange_end)
    {
        const uint8 keyboardKeycode = keycodeInt;
        return IsJustPressed((BindableKeyCode_Keyboard)keyboardKeycode);
    }
    return false;
}
bool IsJustReleased(BindableKeyCode keycode)
{
    constexpr uint16 keyboardRange_start = 1;
    constexpr uint16 keyboardRange_end = 255;
    constexpr uint16 mouseRange_start = (uint16)BindableKeyCode::MOUSE_LMB;
    constexpr uint16 mouseRange_end = (uint16)BindableKeyCode::MOUSE_X5;
    const uint16 keycodeInt = (uint16)keycode;
    if (keycodeInt >= mouseRange_start && keycodeInt <= mouseRange_end)
    {
        const uint8 mouseKeyCode = keycodeInt - mouseRange_start;
        return IsJustReleased((MouseButton)mouseKeyCode);
    }
    if (keycodeInt >= keyboardRange_start && keycodeInt <= keyboardRange_end)
    {
        const uint8 keyboardKeycode = keycodeInt;
        return IsJustReleased((BindableKeyCode_Keyboard)keyboardKeycode);
    }
    return false;
}
bool IsPressed(MouseButton mouseBtn)
{
    return ACU::Input::Get_InputContainerBig()->mouseState.mouseButtonStates[(uint32)mouseBtn];
}
bool IsPressed(ActionKeyCode actionKey)
{
    return InputContainer::GetMainSingleton().keyStates_thisFrame.IsPressed(actionKey);
}

InputHooks* g_InputHooksSingletonPtr = nullptr;
bool IsJustPressed(BindableKeyCode_Keyboard keycode)
{
    return g_InputHooksSingletonPtr->IsJustPressed((uint8)keycode);
}
bool IsJustPressed(MouseButton mouseButton)
{
    return g_InputHooksSingletonPtr->IsJustPressed(mouseButton);
}
bool IsJustPressed(ActionKeyCode actionKey)
{
    auto& inpCont = InputContainer::GetMainSingleton();
    return inpCont.keyStates_thisFrame.IsPressed(actionKey)
        && !inpCont.keyStates_prevFrame.IsPressed(actionKey);
}
bool IsJustReleased(MouseButton mouseButton)
{
    return g_InputHooksSingletonPtr->IsJustReleased(mouseButton);
}
bool IsJustReleased(BindableKeyCode_Keyboard keycode)
{
    return g_InputHooksSingletonPtr->IsJustReleased(keycode);
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

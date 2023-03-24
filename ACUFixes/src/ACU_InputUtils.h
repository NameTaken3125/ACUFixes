#pragma once

#include "ACU/InputContainer.h"
#include "ACU/Enum_ActionKeyCode.h"
#include "Enum_BindableKeyCode_Keyboard.h"

namespace ACU::Input {

inline InputContainerBig* Get_InputContainerBig()
{
    return HasInputContainers::GetSingleton()->p_10->inputContainerBig;
}
bool IsJustPressed(BindableKeyCode_Keyboard keycode);
bool IsJustPressedLong(ActionKeyCode actionKey, float howLong);

} // namespace ACU::Input

inline uint8 ScancodeFromMyKeycode(BindableKeyCode_Keyboard keycode)
{
    return (uint8)keycode;
}
inline bool IsPressed(BindableKeyCode_Keyboard keycode)
{
    return ACU::Input::Get_InputContainerBig()->isPressed_byScancode[(uint32)keycode];
}

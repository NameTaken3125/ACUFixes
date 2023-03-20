#pragma once

#include "ACU/InputContainer.h"
#include "Enum_BindableKeyCode_Keyboard.h"

namespace ACU::Input {
inline InputContainerBig* Get_InputContainerBig()
{
    return HasInputContainers::GetSingleton()->p_10->inputContainerBig;
}
}
inline uint8 ScancodeFromMyKeycode(BindableKeyCode_Keyboard keycode)
{
    return (uint8)keycode;
}
inline bool IsPressed(BindableKeyCode_Keyboard keycode)
{
    return ACU::Input::Get_InputContainerBig()->isPressed_byScancode[(uint32)keycode];
}

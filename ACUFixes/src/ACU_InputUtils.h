#pragma once

#include "ACU/Enum_ActionKeyCode.h"
#include "Enum_BindableKeyCode_Keyboard.h"

class InputContainerBig;

namespace ACU::Input {

InputContainerBig* Get_InputContainerBig();
bool IsPressed(BindableKeyCode_Keyboard keycode);
bool IsJustPressed(BindableKeyCode_Keyboard keycode);
bool IsJustPressedLong(ActionKeyCode actionKey, float howLong);

bool IsPressedRMB();

} // namespace ACU::Input

#pragma once

#include "ACU/Enum_ActionKeyCode.h"
#include "ACU/Enum_MouseButtons.h"
#include "Enum_BindableKeyCode_Keyboard.h"

class InputContainerBig;

namespace ACU::Input {

InputContainerBig* Get_InputContainerBig();
bool IsPressed(BindableKeyCode_Keyboard keycode);
bool IsPressed(MouseButton mouseBtn);
bool IsPressed(ActionKeyCode actionKey);
bool IsJustPressed(BindableKeyCode_Keyboard keycode);
bool IsJustPressed(MouseButton mouseButton);
bool IsJustReleased(BindableKeyCode_Keyboard keycode);
bool IsJustReleased(MouseButton mouseButton);
bool IsJustPressedLong(ActionKeyCode actionKey, float howLong);

bool IsPressedRMB();

} // namespace ACU::Input

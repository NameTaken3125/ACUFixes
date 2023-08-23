#pragma once

#include "ACU/Enum_ActionKeyCode.h"
#include "ACU/Enum_MouseButtons.h"
#include "Common_Plugins/Enum_BindableKeyCode_Keyboard.h"
#include "Common_Plugins/Enum_BindableKeyCode.h"

class InputContainerBig;

namespace ACU::Input {

InputContainerBig* Get_InputContainerBig();
bool IsPressed(BindableKeyCode_Keyboard keycode);
bool IsPressed(BindableKeyCode keycode);
bool IsPressed(MouseButton mouseBtn);
bool IsPressed(ActionKeyCode actionKey);
bool IsJustPressed(BindableKeyCode_Keyboard keycode);
bool IsJustPressed(BindableKeyCode keycode);
bool IsJustPressed(MouseButton mouseButton);
bool IsJustPressed(ActionKeyCode actionKey);
bool IsJustReleased(BindableKeyCode_Keyboard keycode);
bool IsJustReleased(BindableKeyCode keycode);
bool IsJustReleased(MouseButton mouseButton);
bool IsJustPressedLong(ActionKeyCode actionKey, float howLong);
// Returns true if a key was just released after being held down no longer than given amount.
// For example, if you start sprinting and hold down DropBomb button for longer than 0.2 seconds,
// nothing will happen after you release it.
bool IsJustPressedShort(ActionKeyCode actionKey, float noLongerThanThis);

bool IsPressedRMB();

} // namespace ACU::Input

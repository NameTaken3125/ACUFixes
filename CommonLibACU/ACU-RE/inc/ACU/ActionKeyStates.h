#pragma once

#include "Enum_ActionKeyCode.h"

class KeyStates
{
public:
    // @members
    uint8 isPressed_byAction[(int)ActionKeyCode::ACTION_SET_SIZE];

    // @helper_functions
    bool IsPressed(ActionKeyCode action) { return isPressed_byAction[(int)action]; }
}; //Size: 0x0020
assert_sizeof(KeyStates, 0x20);

#pragma once

#include "AIComponent.h"

class CharacterBehavior : public AIComponent
{
public:
    char pad_0B90[80]; //0x0B90
}; //Size: 0x0BE0
assert_sizeof(CharacterBehavior, 0x0BE0);

#pragma once

#include "Component.h"

class Human;

class IAnimationComponent : public Component
{
public:
    char pad_0020[8]; //0x0020
}; //Size: 0x0028
assert_sizeof(IAnimationComponent, 0x28);

class AtomAnimComponent : public IAnimationComponent
{
public:
    char pad_0028[3120]; //0x0028
    Human* human_c58; //0x0C58
    char pad_0C60[16]; //0x0C60
}; //Size: 0x0C70
assert_sizeof(AtomAnimComponent, 0x0C70);

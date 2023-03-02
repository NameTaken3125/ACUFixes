#pragma once

#include "ACU/EventListener.h"

class World;

class UIModule : public ManagedObject
{
public:
    char pad_0010[16]; //0x0010
    EventListener evnListener_20; //0x0020
    char pad_0048[16]; //0x0048
    World* world; //0x0058
    char pad_0060[8]; //0x0060
}; //Size: 0x0068
assert_sizeof(UIModule, 0x68);

class HUDModule : public UIModule
{
public:
    char pad_0068[14]; //0x0068
    uint8 isVisible_76; //0x0076
    char pad_0077[23]; //0x0077
    uint8 isNotCompletelyHidden_8e; //0x008E
    char pad_008F[1]; //0x008F
}; //Size: 0x0090
assert_sizeof(HUDModule, 0x90);

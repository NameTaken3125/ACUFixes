#pragma once

#include "ACU/Event.h"
#include "ACU/ManagedPtrs/AllManagedObjects.h"
#include "ACU/SharedPtr.h"

class OddZoneEvent : public Event
{
public:
    char pad_0010[16]; //0x0010
}; //Size: 0x0020
assert_sizeof(OddZoneEvent, 0x20);

class World;

class HasWorld
{
public:
    // @members
    char pad_0000[144]; //0x0000
    SharedPtrNew<World>* shared_World; //0x0090
    World* world; //0x0098
    char pad_00A0[7224]; //0x00A0
    OddZoneEvent oddZoneEvent; //0x1CD8
    char pad_1CF8[40]; //0x1CF8

    // @helper_functions
    static HasWorld* GetSingleton() { return *(HasWorld**)0x145217368; }
}; //Size: 0x1D20
assert_offsetof(HasWorld, world, 0x98);
assert_sizeof(HasWorld, 0x1D20);

#pragma once

#include "ManagedObject.h"

class World;

class WorldComponent : public ManagedObject
{
public:
    World* world; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020
assert_sizeof(WorldComponent, 0x20);

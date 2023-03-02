#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class BhvAssassin;

class BaseWorld : public ManagedObject
{
public:
    char pad_0010[192]; //0x0010
}; //Size: 0x00D0
assert_sizeof(BaseWorld, 0xD0);

class WorldComponent;
class UIWorldComponent;
class Entity;
class World : public BaseWorld
{
public:
    // @members
    char pad_00D0[8]; //0x00D0
    Entity* player_d8; //0x00D8
    char pad_00E0[8]; //0x00E0
    void* contactManager; //0x00E8
    char pad_00F0[16]; //0x00F0
    void* worldGraphicData; //0x0100
    void* fxManager; //0x0108
    char pad_0110[8]; //0x0110
    SmallArray<WorldComponent*> worldComponents; //0x0118
    char pad_0124[244]; //0x0120
    BhvAssassin** array_toBhvAssassin_218; //0x0218
    char pad_0220[1960]; //0x0220
    UIWorldComponent* uiWorldComponent; //0x09C8
    char pad_09D0[152]; //0x09D0

    // @helper_functions
    static World* GetSingleton();
}; //Size: 0x0A68
assert_offsetof(World, worldComponents, 0x0118);
assert_offsetof(World, array_toBhvAssassin_218, 0x218);
assert_offsetof(World, uiWorldComponent, 0x09C8);
assert_sizeof(World, 0x0A68);

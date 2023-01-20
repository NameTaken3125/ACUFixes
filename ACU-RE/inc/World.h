#pragma once

#include "ManagedObject.h"

class BhvAssassin;

class BaseWorld : public ManagedObject
{
public:
    char pad_0010[192]; //0x0010
}; //Size: 0x00D0
assert_sizeof(BaseWorld, 0xD0);

class World : public BaseWorld
{
public:
    // @members
    char pad_00D0[24]; //0x00D0
    void* contactManager; //0x00E8
    char pad_00F0[16]; //0x00F0
    void* worldGraphicData; //0x0100
    void* fxManager; //0x0108
    char pad_0110[264]; //0x0110
    BhvAssassin** array_toBhvAssassin_218; //0x0218
    char pad_0220[2120]; //0x0220

    // @helper_functions
    static World* GetSingleton();
}; //Size: 0x0A68
assert_sizeof(World, 0x0A68);
assert_offsetof(World, array_toBhvAssassin_218, 0x218);

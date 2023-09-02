#pragma once

#include "basic_types.h"

class RegisteredHotkey
{
public:
    uint32 scancode; //0x0000
    wchar_t buttonName[256]; //0x0004
    char pad_0204[16]; //0x0204
}; //Size: 0x0214
assert_sizeof(RegisteredHotkey, 0x214);

// == RDX at 0x14273BAC0
class KeymapScancodes
{
public:
    char pad_0000[8]; //0x0000
    RegisteredHotkey registeredMappings[32]; //0x0008
    char pad_4288[48]; //0x4288
}; //Size: 0x42B8
assert_offsetof(KeymapScancodes, registeredMappings, 8);

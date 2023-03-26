#pragma once

#include "SoundResource.h"

class SoundEvent : public SoundResource
{
public:
    uint32 dword_10; //0x0010
    float flt_14; //0x0014
    uint8 flags_18; //0x0018
    char pad_0019[7]; //0x0019
}; //Size: 0x0020
assert_sizeof(SoundEvent, 0x20);

class SoundInstance
{
public:
    uint32 flags_mb; //0x0000
    char pad_0004[4]; //0x0004
    SoundEvent soundEvn; //0x0008
    char pad_0028[8]; //0x0028
}; //Size: 0x0030
assert_sizeof(SoundInstance, 0x30);

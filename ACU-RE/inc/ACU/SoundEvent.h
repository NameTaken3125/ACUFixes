#pragma once

#include "SoundResource.h"

class SoundEvent : public SoundResource
{
public:
    char pad_0010[16]; //0x0010
}; //Size: 0x0020
assert_sizeof(SoundEvent, 0x20);

class SoundInstance
{
public:
    char pad_0000[8]; //0x0000
    SoundEvent soundEvn; //0x0008
    char pad_0028[8]; //0x0028
}; //Size: 0x0030
assert_sizeof(SoundInstance, 0x30);

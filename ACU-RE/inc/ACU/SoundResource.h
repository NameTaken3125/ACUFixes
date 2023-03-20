#pragma once

#include "ManagedObject.h"

class SoundResource : public Object
{
public:
    char pad_0008[8]; //0x0008
}; //Size: 0x0010
assert_sizeof(SoundResource, 0x10);

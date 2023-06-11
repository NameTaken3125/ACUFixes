#pragma once

#include "basic_types.h"

class Clock;

class Timer
{
public:
    uint64 timestampStart; //0x0000
    uint64 timestampEnd; //0x0008
    char pad_0010[8]; //0x0010
    Clock* clock; //0x0018
    uint8 isActive_mb_20; //0x0020
    char pad_0021[7]; //0x0021
}; //Size: 0x0028
assert_sizeof(Timer, 0x28);

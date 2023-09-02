#pragma once

#include "SmallArray.h"

// Game code does it like this.
inline float ConvertRawIntTimestampToFloat(int64 rawIntTS) { return rawIntTS / 30000.0f; }

class Clock
{
public:
    // @members
    uint64 currentTimestamp; //0x0000
    char pad_0008[16]; //0x0008
    float deltaTime_mb; //0x0018
    float recentFPS_mb; //0x001C
    float deltaTimeConstant_20; //0x0020
    float overrideTimescale; //0x0024
    float flt_28; //0x0028
    char pad_002C[12]; //0x002C
    uint32 dword_38; //0x0038
    uint32 dword_3C; //0x003C
    uint32 dword_40; //0x0040
    uint8 doOverrideTimescale_mb_44; //0x0044
    uint8 do2ndTimescaleMult_mb_45; //0x0045
    char pad_0046[1]; //0x0046
    SmallArray<Clock*> arrDependentClocks; //0x0047
    char pad_0053[5]; //0x004F

    // @helper_functions

    int64 GetCurrent_RawIntTimestamp() { return currentTimestamp; }
    float GetCurrentTimeFloat() { return ConvertRawIntTimestampToFloat(currentTimestamp); }
    float GetCurrentDeltaTime() { return deltaTime_mb; }
}; //Size: 0x0060
assert_offsetof(Clock, deltaTimeConstant_20, 0x20);
assert_offsetof(Clock, arrDependentClocks, 0x47);
assert_offsetof(Clock, pad_0053, 0x53);
assert_sizeof(Clock, 0x58);

class Timestamp
{
public:
    // @members
    __int64 value; //0x0000
    Clock* clockPtr; //0x0008

    // @helper_functions

    int64 GetCurrent_RawIntTimestamp() { return value ? value : clockPtr->currentTimestamp; }
}; //Size: 0x0010
assert_sizeof(Timestamp, 0x10);

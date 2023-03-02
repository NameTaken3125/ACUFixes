#pragma once

#include "MandatoryUniverseComponent.h"
#include "SmallArray.h"
#include "EventListener.h"

// Game code does it like this.
inline float ConvertRawIntTimestampToFloat(int64 rawIntTS) { return rawIntTS / 30000.0f; }

class Clock
{
public:
    // @members
    __int64 currentTimestamp; //0x0000
    char pad_0008[16]; //0x0008
    float deltaTime_mb; //0x0018
    float recentFPS_mb; //0x001C
    float deltaTimeConstant_20; //0x0020
    char pad_0024[35]; //0x0024
    SmallArray<Clock*> arrClocks; //0x0047
    char pad_004F[17]; //0x004F

    // @helper_functions

    int64 GetCurrent_RawIntTimestamp() { return currentTimestamp; }
    float GetCurrentTimeFloat() { return ConvertRawIntTimestampToFloat(currentTimestamp); }
    float GetCurrentDeltaTime() { return deltaTime_mb; }
}; //Size: 0x0060
assert_offsetof(Clock, deltaTimeConstant_20, 0x20);
assert_offsetof(Clock, arrClocks, 0x47);

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

class GameStatsManager : public MandatoryUniverseComponent
{
public:
    // @members
    char pad_0010[80]; //0x0010
    EventListener evnListener_60; //0x0060
    char pad_0088[336]; //0x0088
    Timestamp clockUnpausedGame; //0x01D8
    char pad_01E8[648]; //0x01E8

    // @helper_functions
    static GameStatsManager* GetSingleton() { return *(GameStatsManager**)0x1451D2878; }
    int64 GetCurrent_RawIntTimestamp() { return clockUnpausedGame.GetCurrent_RawIntTimestamp(); }
    Clock* GetClock_UnpausedGame() { return clockUnpausedGame.clockPtr; }
}; //Size: 0x0470
assert_sizeof(GameStatsManager, 0x470);

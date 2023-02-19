#pragma once

#include "MandatoryUniverseComponent.h"
#include "SmallArray.h"

class EventListener : public ManagedObject
{
public:
    char pad_0010[24]; //0x0010
}; //Size: 0x0028

class Clock
{
public:
    __int64 currentTimestamp; //0x0000
    char pad_0008[63]; //0x0008
    SmallArray<Clock*> arrClocks; //0x0047
    char pad_004F[17]; //0x004F
}; //Size: 0x0060
assert_offsetof(Clock, arrClocks, 0x47);

class Timestamp
{
public:
    // @members
    __int64 value; //0x0000
    class Clock* clockPtr; //0x0008

    // @helper_functions

    // Game code does it like this.
    int64 GetCurrent() { return value ? value : clockPtr->currentTimestamp; }
}; //Size: 0x0010
assert_sizeof(Timestamp, 0x10);

class GameStatsManager : public MandatoryUniverseComponent
{
public:
    // @members
    char pad_0010[80]; //0x0010
    class EventListener evnListener_60; //0x0060
    char pad_0088[336]; //0x0088
    class Timestamp clockUnpausedGame; //0x01D8
    char pad_01E8[648]; //0x01E8

    // @helper_functions
    static GameStatsManager* GetSingleton() { return *(GameStatsManager**)0x1451D2878; }
    float GetCurrentTime_UnpausedGame() { return clockUnpausedGame.GetCurrent() / 30000.0f; }
}; //Size: 0x0470
assert_sizeof(GameStatsManager, 0x470);

#pragma once

#include "MandatoryUniverseComponent.h"
#include "EventListener.h"
#include "Clock.h"

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

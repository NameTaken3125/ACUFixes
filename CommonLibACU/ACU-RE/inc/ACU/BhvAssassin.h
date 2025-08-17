#pragma once

#include "AIComponent.h"

class CharacterBehavior : public AIComponent
{
public:
    char pad_0B90[80]; //0x0B90
}; //Size: 0x0BE0
assert_sizeof(CharacterBehavior, 0x0BE0);

class WhenHighlightedNPCChanges;

class BhvAssassin : public CharacterBehavior
{
public:
    // @members
    char pad_0BE0[2104]; //0x0BE0
    uint8 usedInCheatForceSingleHiddenBlade; //0x1418
    char pad_1419[23]; //0x1419
    WhenHighlightedNPCChanges* toHighlightedNPC; //0x1430
    char pad_1438[8]; //0x1438

    // @helper_functions

    // Memory scan has found a second instance, but this one is from the Player's Entity.
    static BhvAssassin* GetSingletonPlayer();
}; //Size: 0x1440
assert_offsetof(BhvAssassin, toHighlightedNPC, 0x1430);
assert_sizeof(BhvAssassin, 0x1440);

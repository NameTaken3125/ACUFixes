#pragma once

#include "Statechart.h"

class AbstractEntityAI_A8;
class BhvAssassin;
class ReactionHandler;


class AbstractEntityAI : public Statechart
{
public:
    AbstractEntityAI_A8* toInventory_a8; //0x00A8
    char pad_00B0[40]; //0x00B0
    ReactionHandler* reactionHandler; //0x00D8
    void* perceptionHandler; //0x00E0
    void* CLPlayerDeath_; //0x00E8
    char pad_00F0[40]; //0x00F0
    BhvAssassin* bhvAssassin; //0x0118
    char pad_0120[80]; //0x0120
}; //Size: 0x0170
assert_sizeof(AbstractEntityAI, 0x0170);

class AbstractCharacterAI : public AbstractEntityAI
{
public:
    char pad_0170[16]; //0x0170
}; //Size: 0x0180
assert_sizeof(AbstractCharacterAI, 0x0180);

class CharacterAI : public AbstractCharacterAI
{
public:
    char pad_0180[96]; //0x0180
}; //Size: 0x01E0
assert_sizeof(CharacterAI, 0x01E0);

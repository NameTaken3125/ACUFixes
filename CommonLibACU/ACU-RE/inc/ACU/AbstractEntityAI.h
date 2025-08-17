#pragma once

#include "Statechart.h"

class AbstractEntityAI_A8;
class BhvAssassin;
class ReactionHandler;
class ELAbstract;

class AbstractEntityAI : public Statechart
{
public:
    AbstractEntityAI_A8* toInventory_a8; //0x00A8
    char pad_00B0[8]; //0x00B0
    ELAbstract* AutonomousLogic; //0x00B8
    char pad_00C0[24]; //0x00C0
    ReactionHandler* reactionHandler; //0x00D8
    void* perceptionHandler; //0x00E0
    void* CLPlayerDeath_; //0x00E8
    char pad_00F0[40]; //0x00F0
    BhvAssassin* bhvAssassin; //0x0118
    Entity* entity; //0x0120
    char pad_0128[72]; //0x0128
}; //Size: 0x0170
assert_sizeof(AbstractEntityAI, 0x0170);

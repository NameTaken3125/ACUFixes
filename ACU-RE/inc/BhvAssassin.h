#pragma once

#include "Component.h"

class CharacterAI;

class AIComponentBase : public Component
{
public:
    char pad_0020[96]; //0x0020
}; //Size: 0x0080
assert_sizeof(AIComponentBase, 0x80);

class AbstractAIEntityGroup : public ManagedObject
{
public:
    char pad_0010[424]; //0x0010
    char* str_periodicUpdateSetup; //0x01B8
    char pad_01C0[416]; //0x01C0
    char* str_periodicUpdate; //0x0360
    char pad_0368[1808]; //0x0368
}; //Size: 0x0A78
assert_sizeof(AbstractAIEntityGroup, 0x0A78);

class AIComponent : public AIComponentBase
{
public:
    char pad_0080[56]; //0x0080
    CharacterAI* characterAI_toHealthAndInventory; //0x00B8
    char pad_00C0[16]; //0x00C0
    unsigned char bInvisible; //0x00D0
    char pad_00D1[71]; //0x00D1
    AbstractAIEntityGroup abstractAIEntityGroup; //0x0118
}; //Size: 0x0B90
assert_sizeof(AIComponent, 0x0B90);

class CharacterBehavior : public AIComponent
{
public:
    char pad_0B90[80]; //0x0B90
}; //Size: 0x0BE0
assert_sizeof(CharacterBehavior, 0x0BE0);

class BhvAssassin : public CharacterBehavior
{
public:
    // @members
    char pad_0BE0[2144]; //0x0BE0

    // @helper_functions

    // Memory scan has found a second instance, but this one is from the Player's Entity.
    static BhvAssassin* GetSingletonPlayer();
}; //Size: 0x1440
assert_sizeof(BhvAssassin, 0x1440);

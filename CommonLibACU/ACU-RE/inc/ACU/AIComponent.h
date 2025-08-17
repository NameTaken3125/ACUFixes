#pragma once

#include "Component.h"

class CharacterAI;
class Human;

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

class AIComponent;
class FocusFlags : public Object
{
public:
    char pad_0008[24]; //0x0008
    AIComponent* bhv; //0x0020
    char pad_0028[8]; //0x0028
}; //Size: 0x0030
assert_sizeof(FocusFlags, 0x30);

class AIComponent : public AIComponentBase
{
public:
    enum class AI_ExecutionPriority : uint32
    {
        AI_Execution_Priority_0 = 0,
        AI_Execution_Priority_1 = 1,
        AI_Execution_Priority_2 = 2,
        AI_Execution_Priority_3 = 3,
        AI_Execution_Priority_4 = 4,
        AI_Execution_Priority_5 = 5,
        AI_EXECUTION_PRIORITY_MAX = 6,
    };

    FocusFlags FocusFlags_; //0x0080
    uint32 ExecutionPriority; //0x00B0
    char pad_00B4[4]; //0x00B4
    CharacterAI* characterAI_toHealthAndInventory; //0x00B8
    char pad_00C0[8]; //0x00C0
    Human* human; //0x00C8
    uint8 bInvisible; //0x00D0
    char pad_00D1[63]; //0x00D1
    uint8 byte_110; //0x0110
    char pad_0111[7]; //0x0111
    AbstractAIEntityGroup abstractAIEntityGroup; //0x0118
}; //Size: 0x0B90
assert_offsetof(AIComponent, byte_110, 0x110);
assert_sizeof(AIComponent, 0x0B90);

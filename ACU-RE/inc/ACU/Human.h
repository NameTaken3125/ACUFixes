#pragma once

#include "ManagedObject.h"
#include "SharedPtr.h"

class AIActor : public Object
{
public:
    char pad_0008[448]; //0x0008
}; //Size: 0x01C8
assert_sizeof(AIActor, 0x1C8);

class HumanStatesHolder;
class HumanBase_StealthData;
class EntityGroup;
class HumanBase : public AIActor
{
public:
    void* usedDuringAddGearLantern; //0x01C8
    char pad_01D0[136]; //0x01D0
    SharedPtrNew<EntityGroup>* lanternEntityGroupWhenLanternIsPulledOutAtLeastOnce; //0x0258
    char pad_0260[1200]; //0x0260
    HumanStatesHolder* humanStates; //0x0710
    char pad_0718[16]; //0x0718
    HumanBase_StealthData* stealthData; //0x0728
    char pad_0730[608]; //0x0730
}; //Size: 0x0990
assert_offsetof(HumanBase, humanStates, 0x710);
assert_sizeof(HumanBase, 0x990);

class Human : public HumanBase
{
public:
    char pad_0990[1744]; //0x0990
}; //Size: 0x1060
assert_sizeof(Human, 0x1060);

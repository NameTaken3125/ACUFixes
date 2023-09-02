#pragma once


#include "ManagedObject.h"
#include "SharedPtr.h"

class AIActor : public Object
{
public:
    char pad_0008[448]; //0x0008
}; //Size: 0x01C8
assert_sizeof(AIActor, 0x1C8);

#include "SmallArray.h"
class SkeletonComponent;
class Entity;
class HumanBase_358_0
{
public:
    SharedPtrNew<SkeletonComponent>* shared_skeletonCpnt; //0x0000
    char pad_0008[8]; //0x0008
    SharedPtrNew<Entity>* shared_entity; //0x0010
}; //Size: 0x0018
assert_sizeof(HumanBase_358_0, 0x18);
class HumanBase_358
{
public:
    SmallArray<HumanBase_358_0> arrSmthWithSkeletonComponentsAndEntities; //0x0000
    char pad_000C[12]; //0x000C
}; //Size: 0x0018
assert_sizeof(HumanBase_358, 0x18);

class HumanStatesHolder;
class HumanBase_StealthData;
class EntityGroup;
class HumanAtomData;
#include <array>
class HumanBase : public AIActor
{
public:
    void* usedDuringAddGearLantern; //0x01C8
    char pad_01D0[24]; //0x01D0
    Entity* entity; //0x01E8
    char pad_01F0[40]; //0x01F0
    HumanAtomData* humanAtomData; //0x0218
    char pad_0220[56]; //0x0220
    SharedPtrNew<EntityGroup>* lanternEntityGroupWhenLanternIsPulledOutAtLeastOnce; //0x0258
    char pad_0260[248]; //0x0260
    std::array<HumanBase_358, 34> arr358; //0x0358
    char pad_0688[136]; //0x0688
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
    // @members
    char pad_0990[1744]; //0x0990

    // @helper_functions
    static Human* GetForPlayer();
}; //Size: 0x1060
assert_sizeof(Human, 0x1060);

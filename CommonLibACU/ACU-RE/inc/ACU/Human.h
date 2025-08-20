#pragma once


#include "ManagedObject.h"
#include "SharedPtr.h"

class AIActor : public Object
{
public:
    // @vtbl

    virtual void Unk000() override;
    virtual void Unk008() override;
    virtual void Unk010_Deserialize(DeserializationStream* deserializationStream) override;
    virtual Object* Unk018_Clone(uint64 a2, uint32 a3) override;
    virtual void Unk020() override;
    virtual TypeInfo& Unk028_GetTI() override;
    virtual void Unk030();
    virtual void Unk038();
    virtual void Unk040();
    virtual void Unk048();
    virtual void Unk050();
    virtual void Unk058();
    virtual void Unk060();
    virtual void Unk068();
    virtual void Unk070();
    virtual void Unk078();
    virtual void Unk080();
    virtual void Unk088();
    virtual void Unk090();
    virtual void Unk098();
    virtual void Unk0A0();
    virtual void Unk0A8();
    virtual void Unk0B0();
    virtual void Unk0B8();
    virtual void Unk0C0();
    virtual void Unk0C8();
    virtual void Unk0D0();
    virtual void Unk0D8();
    virtual void Unk0E0();
    virtual void Unk0E8();
    virtual void* Unk0F0_GetInterface(int interfaceIdx);

    // @members
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
class World;

class Human1C8
{
public:
    virtual void Unk000();
    virtual void Unk008();
    virtual void Unk010();
    virtual void Unk018();
    virtual void Unk020();
    virtual void Unk028();
    virtual void Unk030();
    virtual void Unk038();
    virtual void Unk040(World* world);
    virtual void Unk048();
    virtual void Unk050();
    virtual void Unk058();
};
class HumanBase : public AIActor
{
public:
    Human1C8 usedDuringAddGearLantern; //0x01C8
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
assert_offsetof(HumanBase, usedDuringAddGearLantern, 0x1C8);
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

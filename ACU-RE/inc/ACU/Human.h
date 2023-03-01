#pragma once

#include "ManagedObject.h"

class AIActor : public Object
{
public:
    char pad_0008[448]; //0x0008
}; //Size: 0x01C8
assert_sizeof(AIActor, 0x1C8);

class HasBallisticAimingEquipmentType;
class HumanBase : public AIActor
{
public:
    char pad_01C8[1352]; //0x01C8
    HasBallisticAimingEquipmentType* hasBallisticAimingEquipmentType_710; //0x0710
    char pad_0718[632]; //0x0718
}; //Size: 0x0990
assert_offsetof(HumanBase, hasBallisticAimingEquipmentType_710, 0x710);
assert_sizeof(HumanBase, 0x990);

class Human : public HumanBase
{
public:
    char pad_0990[1744]; //0x0990
}; //Size: 0x1060
assert_sizeof(Human, 0x1060);

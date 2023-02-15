#pragma once

#include "BaseEntity.h"

class Component;

class Entity_15F_CpntIndices
{
public:
    unsigned char byte_15f; //0x0000
    unsigned char cpntIdx_BhvAssassin_or_AIComponent; //0x0001
    char pad_0002[3]; //0x0002
}; //Size: 0x0005
assert_sizeof(Entity_15F_CpntIndices, 5);

class Entity : public BaseEntity
{
public:
    Component* cpnts_mb; //0x0078
    char pad_0080[223]; //0x0080
    Entity_15F_CpntIndices cpntIndices; //0x015F
    char pad_0164[12]; //0x0164
}; //Size: 0x0170
assert_sizeof(Entity, 0x0170);

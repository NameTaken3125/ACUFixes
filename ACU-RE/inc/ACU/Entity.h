#pragma once

#include "BaseEntity.h"
#include "SmallArray.h"

class Component;

class Entity_CpntIndices_157
{
public:
	char pad_0000[1]; //0x0000
	uint8 atomAnimCpnt; //0x0001
	char pad_0002[6]; //0x0002
}; //Size: 0x0008

class Entity_CpntIndices_15F
{
public:
	uint8 byte_15f; //0x0000
	uint8 cpntIdx_BhvAssassin_or_AIComponent; //0x0001
	char pad_0002[3]; //0x0002
}; //Size: 0x0005
assert_sizeof(Entity_CpntIndices_15F, 5);

class Entity : public BaseEntity
{
public:
	// @members
    SmallArray<Component*> cpnts_mb; //0x0078
    char pad_0084[211]; //0x0080
    Entity_CpntIndices_157 cpntIndices_157; //0x0157
    Entity_CpntIndices_15F cpntIndices; //0x015F
    char pad_0164[12]; //0x0164

	// @helper_functions
	Component* FindComponentByVTBL(uint64 vtbl);
}; //Size: 0x0170
assert_offsetof(Entity, cpntIndices_157.atomAnimCpnt, 0x158);
assert_sizeof(Entity, 0x0170);

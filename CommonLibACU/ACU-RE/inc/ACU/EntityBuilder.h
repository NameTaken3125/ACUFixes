#pragma once

#include "BuildTable.h"

class EntityBuilder : public BuildTable
{
public:
	SharedPtrNew<Entity>* Template; //0x00E8
	SharedPtrNew<Entity>* ReplicaTemplate; //0x00F0
	SmallArray<SharedPtrNew<BuildTable>*> Tables; //0x00F8
	char pad_0104[4]; //0x0104
}; //Size: 0x0108
assert_sizeof(EntityBuilder, 0x108);

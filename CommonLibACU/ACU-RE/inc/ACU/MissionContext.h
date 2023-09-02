#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class MissionGroup;
class MissionRoot;

class MissionContext : public ManagedObject
{
public:
	SmallArray<MissionGroup*> missionGroups; //0x0010
	char pad_001C[4]; //0x001C
	MissionRoot* missionRoot; //0x0020
	char pad_0028[12]; //0x0028
	uint8 byte_34; //0x0034
	char pad_0035[3]; //0x0035
}; //Size: 0x0038
assert_sizeof(MissionContext, 0x38);

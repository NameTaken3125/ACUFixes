#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"
#include "MissionReplicationPack.h"

class MissionRoot;
class MissionContext;
class MissionConditionGroup;
class MissionGroupActionPack;

class MissionGroup : public ManagedObject
{
public:
	char pad_0010[17]; //0x0010
	SmallArray<void*> arr21; //0x0021
	char pad_002D[11]; //0x002D
	MissionRoot* missionRoot; //0x0038
	MissionContext* missionContext; //0x0040
	char pad_0048[8]; //0x0048
	MissionConditionGroup* missionConditionGroup_50; //0x0050
	MissionConditionGroup* missionConditionGroup_58; //0x0058
	SharedPtrNew<MissionGroupActionPack>* shared_missionGroupActionPack; //0x0060
	SmallArray<SharedPtrNew<Mission>*> missions; //0x0068
	char pad_0074[4]; //0x0074
	MissionReplicationPack missionReplicationPack; //0x0078
	char pad_0088[16]; //0x0088
}; //Size: 0x0098
assert_sizeof(MissionGroup, 0x98);

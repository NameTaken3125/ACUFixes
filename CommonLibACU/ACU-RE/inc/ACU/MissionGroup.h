#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"
#include "MissionReplicationPack.h"

class MissionRoot;
class MissionContext;
class MissionCondition;
class MissionGroupActionPack;

enum class MissionHistoryTrackIdentifiers : uint32
{
	MissionHistoryTrack_Narrative = 0,
};

class MissionGroup : public ManagedObject
{
public:
	char pad_0010[17]; //0x0010
	SmallArray<void*> arr21; //0x0021
	char pad_002D[11]; //0x002D
	MissionRoot* Root; //0x0038
	MissionContext* Context; //0x0040
	MissionHistoryTrackIdentifiers MissionHistoryTrackID; //0x0048
	char pad_004C[4]; //0x004C
	MissionCondition* UnlockCondition; //0x0050
	MissionCondition* CompleteCondition; //0x0058
	SharedPtrNew<MissionGroupActionPack>* ActionPack; //0x0060
	SmallArray<SharedPtrNew<Mission>*> Missions; //0x0068
	char pad_0074[4]; //0x0074
	MissionReplicationPack ReplicationPack; //0x0078
	char pad_0088[16]; //0x0088
}; //Size: 0x0098
assert_sizeof(MissionGroup, 0x98);

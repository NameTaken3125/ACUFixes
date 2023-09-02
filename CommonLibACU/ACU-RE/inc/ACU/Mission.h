#pragma once

#include "ManagedObject.h"
#include "SharedPtr.h"
#include "MissionReplicationPack.h"


class MissionGroup;
class MissionStepGroup;
class MissionConditionGroup;
class MissionActionPack;
class MissionCategory;
class DataLayer;
class ResetMissionConstraint;
class World;
class WorldTransitionPortal;

class Mission : public ManagedObject
{
public:
	char pad_0010[0x21-0x10]; //0x0010
	SmallArray<void*> arr21; //0x0021
	char pad_002D[19]; //0x002D
	uint64 qword_40; //0x0040
	char pad_0048[8]; //0x0048
	MissionGroup* missionGroup; //0x0050
	MissionReplicationPack missionReplicationPack; //0x0058
	char pad_0068[8]; //0x0068
	SharedPtrNew<MissionCategory>* shared_MissionCategory; //0x0070
	char pad_0078[8]; //0x0078
	MissionConditionGroup* missionConditionGroup; //0x0080
	SharedPtrNew<MissionStepGroup>* shared_missionStepGroup_88; //0x0088
	SharedPtrNew<DataLayer>* shared_DataLayer_90; //0x0090
	SharedPtrNew<MissionStepGroup>* shared_missionStepGroup_98; //0x0098
	SharedPtrNew<DataLayer>* shared_DataLayer_A0; //0x00A0
	SharedPtrNew<DataLayer>* shared_DataLayer_A8; //0x00A8
	char pad_00B0[24]; //0x00B0
	SharedPtrNew<DataLayer>* shared_DataLayer_C8; //0x00C8
	char pad_00D0[16]; //0x00D0
	ResetMissionConstraint* resetMissionConstraint; //0x00E0
	SharedPtrNew<MissionActionPack>* shared_missionActionPack; //0x00E8
	SharedPtrNew<World>* shared_world; //0x00F0
	SharedPtrNew<WorldTransitionPortal>* shared_WorldTransitionPortal; //0x00F8
	char pad_0100[144]; //0x0100
	SharedPtrNew<MissionActionPack>* shared_DataLayer_190; //0x0190
	char pad_0198[8]; //0x0198
	void* usedDuringAcceptMission; //0x01A0
}; //Size: 0x01A8
assert_sizeof(Mission, 0x1A8);

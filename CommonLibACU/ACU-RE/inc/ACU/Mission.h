#pragma once

#include "ManagedObject.h"
#include "SharedPtr.h"
#include "MissionReplicationPack.h"


class MissionGroup;
class MissionStep;
class MissionStepGroup;
class MissionCondition;
class MissionConditionGroup;
class MissionActionPack;
class MissionCategory;
class MissionConstraint;
class DataLayer;
class ResetMissionConstraint;
class World;
class IWorldTransitionPortal;
class WorldTransitionPortal;
class WorldTransitionProfileContainer;
class SoundBanksLoadOnDemand;

class Mission : public ManagedObject
{
public:
	char pad_0010[0x21-0x10]; //0x0010
	SmallArray<void*> arr21; //0x0021
	char pad_002D[19]; //0x002D
	uint64 qword_40; //0x0040
	SharedPtrNew<WorldTransitionProfileContainer>* MissionTransitionProfileContainer; //0x0048
	MissionGroup* Group; //0x0050
	MissionReplicationPack ReplicationPack; //0x0058
	SharedPtrNew<SoundBanksLoadOnDemand>* SoundBanksIntroLayer; //0x0068
	SharedPtrNew<MissionCategory>* Category; //0x0070
	SharedPtrNew<SoundBanksLoadOnDemand>* SoundBanksBodyLayer; //0x0078
	MissionCondition* UnlockCondition; //0x0080
	SharedPtrNew<MissionStep>* IntroStep; //0x0088
	SharedPtrNew<DataLayer>* IntroLayer; //0x0090
	SharedPtrNew<MissionStep>* BodyStep; //0x0098
	SharedPtrNew<DataLayer>* BodyLayer; //0x00A0
	SharedPtrNew<DataLayer>* BodyCrowdlifeLayer; //0x00A8
	SharedPtrNew<DataLayer>* MultiplayerBodyLayers[3]; //0x00B0
	SharedPtrNew<DataLayer>* IntroBodyLayer; //0x00C8
	SmallArray<MissionConstraint*> Constraints; //0x00D0
	char pad_00DC[4]; //0x00DC
	ResetMissionConstraint* ResetConstraints; //0x00E0
	SharedPtrNew<MissionActionPack>* ActionPack; //0x00E8
	SharedPtrNew<World>* AssociatedWorld; //0x00F0
	SharedPtrNew<IWorldTransitionPortal>* MissionTransitionPortal; //0x00F8
	char pad_0100[4]; //0x0100
	UIString MissionNameUIString; //0x0104
	UIString MissionDescriptionUIString; //0x0108
	uint32 MissionCompletedSyncValue; //0x010C
	uint32 FullSyncBonusValue; //0x0110


	// 0x114

	uint16 BackToBack : 1;
	uint16 KeepBodyLayer : 1;
	uint16 SaveOnSuccess : 1;
	uint16 ReInitOnWorldChange : 1;
	uint16 UseTransitionFXOnBriefing : 1;
	uint16 UseTransitionFXOnReward : 1;
	uint16 ShowMissionCompleteAFS : 1;
	uint16 CanBeAborted : 1;

	uint16 PauseMissionCompleteAFS : 1;
	uint16 DesynchOnFailure : 1;
	uint16 ReinitOnScaleDown : 1;
	uint16 CanBeRelocked : 1;
	uint16 UseMatchmaking : 1;

	char pad_0116[0x190-0x116]; //0x0116
	SharedPtrNew<MissionActionPack>* shared_DataLayer_190; //0x0190
	char pad_0198[8]; //0x0198
	void* usedDuringAcceptMission; //0x01A0
}; //Size: 0x01A8
assert_offsetof(Mission, shared_DataLayer_190, 0x190);
assert_sizeof(Mission, 0x1A8);

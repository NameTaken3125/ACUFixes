#pragma once

#include "MandatoryUniverseComponent.h"
#include "EventListener.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class MissionRoot;
class MissionHistory;
class MissionGroup;
class Mission;
class MissionStepGroup;
class DataLayer;


class MissionManager_68
{
public:
	char pad_0000[48]; //0x0000
}; //Size: 0x0030
assert_sizeof(MissionManager_68, 0x30);


class MissionManager : public MandatoryUniverseComponent
{
public:
	// @members
	char pad_0010[8]; //0x0010
	SharedPtrNew<MissionRoot>* shared_MissionRoot; //0x0018
	SmallArray<MissionHistory> missionHistories; //0x0020
	char pad_002C[60]; //0x002C
	MissionManager_68 stru68; //0x0068
	MissionManager_68 stru98; //0x0098
	MissionManager_68 struC8; //0x00C8
	MissionManager_68 struF8; //0x00F8
	MissionManager_68 stru128; //0x0128
	MissionManager_68 stru158; //0x0158
	MissionManager_68 stru188; //0x0188
	MissionManager_68 stru1B8; //0x01B8
	SmallArray<void*> arr1E8; //0x01E8
	SmallArray<void*> arr1F4; //0x01F4
	SmallArray<MissionGroup*> missionGroups200_locked_mb; //0x0200
	SmallArray<MissionGroup*> missionGroups20C; //0x020C
	SmallArray<MissionGroup*> missionGroups218; //0x0218
	SmallArray<Mission*> missions224_grayNoncompletedAndReachableAtThisStage; //0x0224
	SmallArray<Mission*> missions230; //0x0230
	SmallArray<Mission*> missions23C_acceptable; //0x023C
	SmallArray<Mission*> missions248_active_mb; //0x0248
	SmallArray<Mission*> missions254_completedAll_mb; //0x0254
	SmallArray<MissionStepGroup*> missionStepGroups260; //0x0260
	SmallArray<MissionStepGroup*> missionStepGroups26C; //0x026C
	char pad_0278[0xC]; //0x0278
	SmallArray<SharedPtrNew<DataLayer>*> dataLayers; //0x0284
	char pad_0290[96]; //0x0290
	uint64 shared2F0; //0x02F0
	char pad_02F8[56]; //0x02F8
	EventListener evnListener_330; //0x0330
	char pad_0358[8]; //0x0358
	MissionManager* missionManager_360; //0x0360
	char pad_0368[64]; //0x0368
	uint64 missionRootHandle; //0x03A8
	SmallArray<SharedPtrNew<MissionGroup>*> dlcMissionGroups_mb_3B0; //0x03B0
	char pad_03BC[4]; //0x03BC

	// @helper_functions
	static MissionManager* GetSingleton() { return *(MissionManager**)0x14521A210; }
}; //Size: 0x03C0
assert_offsetof(MissionManager, missionGroups200_locked_mb, 0x200);
assert_offsetof(MissionManager, dlcMissionGroups_mb_3B0, 0x3B0);
assert_sizeof(MissionManager, 0x3C0);

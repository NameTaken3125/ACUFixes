#pragma once

#include "ManagedObject.h"
#include "SharedPtr.h"
#include "SmallArray.h"

class Mission;
class MissionStepSaveData;

class MissionSaveData : public Object
{
public:
	SharedPtrNew<Mission>* shared_mission; //0x0008
	char pad_0010[12]; //0x0010
	SmallArray<MissionStepSaveData*> arrMissionStepSaveData; //0x001C
	SmallArray<void*> arr28; //0x0028
	char pad_0034[36]; //0x0034
}; //Size: 0x0058
assert_sizeof(MissionSaveData, 0x58);

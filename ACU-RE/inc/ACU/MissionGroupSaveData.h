#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class MissionSaveData;
class MissionGroup;

class MissionGroupSaveData : public Object
{
public:
	SharedPtrNew<MissionGroup>* shared_MissionGroup; //0x0008
	SmallArray<MissionSaveData*>arrMissionSaveData; //0x0010
	char pad_001C[4]; //0x001C
}; //Size: 0x0020
assert_sizeof(MissionGroupSaveData, 0x20);

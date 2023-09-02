#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class MissionGroupSaveData;
class WorldTransitionPortal;

class MissionHistory : public ManagedObject
{
public:
	char pad_0010[4]; //0x0010
	SmallArray<MissionGroupSaveData*> arrMissionGroupSaveData; //0x0014
	SharedPtrNew<WorldTransitionPortal>* shared_WorldTransitionPortal; //0x0020
	char pad_0028[56]; //0x0028
}; //Size: 0x0060
assert_sizeof(MissionHistory, 0x60);

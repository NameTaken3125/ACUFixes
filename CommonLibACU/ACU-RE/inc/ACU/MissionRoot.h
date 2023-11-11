#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class MissionGroup;
class MissionContext;
class MissionCategory;

class MissionRoot : public ManagedObject
{
public:
	// @members
	char pad_0010[17]; //0x0010
	SmallArray<void*> arr21; //0x0021
	char pad_002D[3]; //0x0029
	SmallArray<SharedPtrNew<MissionGroup>*> missionGroups_all_mb; //0x0030
	SmallArray<MissionContext*> arrMissionCtx; //0x003C
	SmallArray<MissionCategory*> arrMissionCategories; //0x0048
	char pad_0054[4]; //0x0054

	// @helper_functions
	static MissionRoot* GetSingleton();
}; //Size: 0x0058
assert_sizeof(MissionRoot, 0x58);

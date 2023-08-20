#pragma once

#include "ManagedObject.h"
#include "SharedPtr.h"
#include "SmallArray.h"

class Mission;
class MissionStepActionPack;

class MissionStep : public ManagedObject
{
public:
	char pad_0010[17]; //0x0010
	SmallArray<void*> arr21; //0x0021
	char pad_002D[11]; //0x002D
	Mission* mission; //0x0038
	char pad_0040[16]; //0x0040
	SharedPtrNew<MissionStepActionPack>* shared_missionStepActionPack_50; //0x0050
	char pad_0058[48]; //0x0058
	SharedPtrNew<MissionStepActionPack>* shared_missionStepActionPack_88; //0x0088
	char pad_0090[16]; //0x0090
}; //Size: 0x00A0
assert_sizeof(MissionStep, 0xA0);

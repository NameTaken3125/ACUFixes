#pragma once

#include "ManagedObject.h"
#include "SharedPtr.h"

class MissionStep;

class MissionStepSaveData : public Object
{
public:
	SharedPtrNew<MissionStep>* shared_missionStep; //0x0008
	char pad_0010[8]; //0x0010
}; //Size: 0x0018
assert_sizeof(MissionStepSaveData, 0x18);

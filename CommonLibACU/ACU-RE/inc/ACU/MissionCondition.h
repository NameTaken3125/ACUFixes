#pragma once

#include "ManagedObject.h"


class Mission;
class MissionStep;

class MissionConditionBase : public Object
{
public:
	char pad_0008[32]; //0x0008
	Mission* ParentMission; //0x0028
	MissionStep* ParentMissionStep; //0x0030
	uint8 Negated; //0x0038
	char pad_0039[47]; //0x0039
}; //Size: 0x0068
assert_sizeof(MissionConditionBase, 0x68);

class MissionCondition : public MissionConditionBase
{
public:
}; //Size: 0x0068
assert_sizeof(MissionCondition, 0x68);

#pragma once

#include "MissionCondition.h"
#include "SmallArray.h"

class MissionConditionOperator;

enum class MissionOperatorType : uint32
{
	MissionOperator_All = 1,
	MissionOperator_Any = 2,
};
class MissionConditionGroup : public MissionCondition
{
public:
	char pad_0068[8]; //0x0068
	MissionOperatorType OperatorType; //0x0070
	char pad_0074[4]; //0x0074
	MissionConditionOperator* Operator; //0x0078
	SmallArray<MissionCondition*> Conditions; //0x0080
	char pad_008C[4]; //0x008C
}; //Size: 0x0090
assert_sizeof(MissionConditionGroup, 0x90);

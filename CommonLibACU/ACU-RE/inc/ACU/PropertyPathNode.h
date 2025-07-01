#pragma once

#include "basic_types.h"

class PropertyPathNodeSolver;

class PropertyPathNode
{
public:
	enum class NodeCondition : uint32
	{
		NC_NOCONDITION = 0,
	};

	PropertyPathNodeSolver* NodeSolver; //0x0000

	uint32 Index : 16;
	NodeCondition Condition : 4;
	NodeCondition Condition2 : 4;
	uint32 MatchAllConditions : 1;
	uint32 ArrayMultiTargetting : 1;
	uint32 Optionnal : 1;

	int32 TargetPropertyID; //0x000C
	int32 ConditionPropertyID; //0x0010
	int32 Condition2PropertyID; //0x0014
	uint64 ConditionData; //0x0018
	uint64 Condition2Data; //0x0020
}; //Size: 0x0028
assert_sizeof(PropertyPathNode, 0x28);

#pragma once

#include "AtomDataContainerWrapper.h"

enum class AtomCondition_ConditionType : uint16
{
	GRAPH_VARIABLE = 0,
	MarkUpCondition = 1,
	CONDITION_GROUP = 2,
	PLAYBACK_PERCENTAGE_mb = 3,
	AccumulatedMarkUpCondition = 4,
};
enum class AtomCondition_ConditionalOperator : uint16
{
	EQUALS = 0,
	GREATER_THAN = 1,
	GREATER_EQUAL = 2,
	LESS_THAN = 3,
	LESS_EQUAL = 4,
	NOT_EQUAL = 5,
};

enum class AtomCondition_ConjunctionOperator : uint16
{
	AND = 0,
	OR = 1,
};

enum class AtomMarkUpQueryScope : uint16
{
	AtomMarkUpQuery_Set = 0,
	AtomMarkUpQuery_NotSet = 2,
};

class AtomCondition
{
public:
	// @members
	uint32 ValueToTestReferenceID; //0x0000
	uint32 MarkUpLayerScope; //0x0004
	AtomCondition_ConditionType ConditionType : 3; //0x0008
	AtomCondition_ConditionalOperator ConditionalOperator : 4;
	AtomCondition_ConjunctionOperator ConjunctionOperator : 2;
	AtomMarkUpQueryScope MarkUpQueryScope : 2;
	uint16 ComparisonValueIsRTCP : 1; // Take a look at 14009EEA4
	uint16 bit_padding_12_15 : 4;
	uint16 SourceEntityRTCP; //0x000A // It's usually ==0xFFFF, but maybe can be the index of an "entityref" RTCP variable whose GraphEvaluation is supposed to be used?
    // 0x000C
	// Appears to represent something related to "parentheses" within the `AtomConditionExpression`.
    // Like, how many of the conditions that follow this one need to be "skipped".
    // If so, then it would make sense that the last condition in an expression should have this value at 0.
	uint16 SubEntryCount_groupSizeIfDescribesConditionGroup; //0x000C
	char pad_000E[2]; //0x000E
	AtomDataContainerWrapper ComparisonValue; //0x0010

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D8500; }
}; //Size: 0x0030
assert_sizeof(AtomCondition, 0x30);

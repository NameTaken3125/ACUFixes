#pragma once

#include "basic_types.h"

enum class AtomDataContainerWrapper_DataType : uint32
{
	Bool = 0,
	Float = 1,
	Int = 2,
	XY = 4,
	XYZ = 5,
	XYZW = 6,

	Weird_Datatype_In_Weird_Conditions = 10,
};

class AtomDataContainerWrapper
{
public:
	int32 value; //0x0000
	char pad_0004[12]; //0x0004
	AtomDataContainerWrapper_DataType DataType_0bool1float2int4xy5xyz6quat; //0x0010
	char pad_0014[12]; //0x0014
}; //Size: 0x0020
assert_sizeof(AtomDataContainerWrapper, 0x20);

enum class AtomCondition_ConditionType : uint16
{
	GRAPH_VARIABLE = 0,
	UNK_1 = 1,
	WEIRD_CONDITION = 2,
	PLAYBACK_PERCENTAGE_mb = 3,
	UNK_4 = 4,
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

class AtomCondition
{
public:
	// @members
	uint32 ValueToTestReferenceID; //0x0000
	uint32 dword4; //0x0004
	AtomCondition_ConditionType ConditionType : 3; //0x0008
	AtomCondition_ConditionalOperator ConditionalOperator : 4;
	AtomCondition_ConjunctionOperator ConjunctionOperator : 2;
	uint16 MarkUpQueryScope : 2;
	uint16 isComparisonValueAnAnotherRTCPVarIndex : 1; // Take a look at 14009EEA4
	uint16 bit_padding_12_15 : 4;
	uint16 word_A; //0x000A // It's usually ==0xFFFF, but maybe can be the index of an "entityref" RTCP variable whose GraphEvaluation is supposed to be used?
	uint16 word_C; //0x000C
	char pad_000E[2]; //0x000E
	AtomDataContainerWrapper ComparisonValue; //0x0010

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D8500; }
}; //Size: 0x0030
assert_sizeof(AtomCondition, 0x30);

#pragma once

#include "basic_types.h"
#include "SmallArray.h"

class AtomCondition;

class AtomConditionExpression
{
public:
	// @members
	uint64 ExpressionID; //0x0000
	SmallArray<uint32> referencedGraphVarsIndices; //0x0008
	SmallArray<AtomCondition> Conditions; //0x0014
	char pad_0020[8]; //0x0020

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D85B0; }
}; //Size: 0x0028
assert_sizeof(AtomConditionExpression, 0x28);

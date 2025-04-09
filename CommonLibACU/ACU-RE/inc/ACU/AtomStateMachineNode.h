#pragma once

#include "AtomStateNode.h"
#include "AtomNodeID.h"

class AtomConditionExpression;

class AtomInitialState
{
public:
	// @members
	AtomConditionExpression* conditionExpression; //0x0000
	uint16 TargetStateIndex; //0x0008
	AtomNodeID nodeID; //0x000A
	char pad_001A[6]; //0x001A

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439DD730; }
}; //Size: 0x0020
assert_sizeof(AtomInitialState, 0x20);

class AtomStateMachineNode : public AtomStateNode
{
public:
	// @members
	SmallArray<AtomStateNode*> States; //0x0080
	SmallArray<AtomInitialState> InitialStates; //0x008C
	SmallArray<AtomStateTransitionTarget*> GlobalTransitions; //0x0098
	char pad_00A4[4]; //0x00A4

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439DD890; }
}; //Size: 0x00A8
assert_sizeof(AtomStateMachineNode, 0xA8);

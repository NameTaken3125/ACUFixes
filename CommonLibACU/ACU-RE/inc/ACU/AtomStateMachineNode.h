#pragma once

#include "AtomStateNode.h"

class AtomInitialState;

class AtomStateMachineNode : public AtomStateNode
{
public:
	SmallArray<AtomStateNode*> States; //0x0080
	SmallArray<AtomInitialState> InitialStates; //0x008C
	SmallArray<AtomStateTransitionTarget*> transitionTargets_98; //0x0098
	char pad_00A4[4]; //0x00A4
}; //Size: 0x00A8
assert_sizeof(AtomStateMachineNode, 0xA8);

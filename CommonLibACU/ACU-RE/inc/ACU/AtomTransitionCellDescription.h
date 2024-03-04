#pragma once

#include "AtomNodeID.h"

class AtomStateMachineNode;

class AtomTransitionCellDescription
{
public:
	AtomStateMachineNode* stateMachineNode; //0x0000
	char pad_0008[12]; //0x0008
	AtomNodeID nodeID_14; //0x0014
	AtomNodeID nodeID_24; //0x0024
	AtomNodeID nodeID_34; //0x0034
	AtomNodeID nodeID_44; //0x0044
	char pad_0054[4]; //0x0054
}; //Size: 0x0058
assert_sizeof(AtomTransitionCellDescription, 0x58);

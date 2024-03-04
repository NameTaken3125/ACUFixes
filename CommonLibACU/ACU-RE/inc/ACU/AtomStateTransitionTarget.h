#pragma once

#include "ManagedObject.h"
#include "AtomNodeID.h"

class AtomConditionExpression;

class AtomStateTransitionTarget : public Object
{
public:
	// @members
	AtomConditionExpression* conditionExpression; //0x0008
	AtomNodeID nodeID; //0x0010
	float TransitionTime; //0x0020
	uint16 TargetStateIndex; //0x0024
	uint16 BlendType : 3;
	uint16 bits_26_3_4 : 2;
	uint16 bits_26_5_6_prop0x9B4DCC4C : 2;
	uint16 bits_26_7_8_prop0xBC8FC6BB : 2;
	uint16 bits_26_9_10_prop0x833FDA8B : 2;
	uint16 bit_26_11_prop0xA228BF61 : 1;
	uint16 bit_26_12_prop0xECB5137C : 1;
	uint16 bit_26_13_prop0x390CAD45 : 1;
	uint16 bitpadding_26_14 : 1;
	uint16 bit_26_15_prop0x8481186F : 1;

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439DDDB0; }
}; //Size: 0x0028
assert_sizeof(AtomStateTransitionTarget, 0x28);

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
	uint16 BlendType : 3; // Enum ACTBlendType
	uint16 BlendAlgo : 2; // Enum ACTBlendAlgo
	uint16 TranslationSource : 2; // Enum ACTBlendDispSrcMode
	uint16 RotationSource : 2; // Enum ACTBlendDispSrcMode
	uint16 ActorRef1Source : 2; // Enum ACTBlendDispSrcMode
	uint16 UseTransitionMatrix : 1;
	uint16 ForceProportional : 1;
	uint16 InverseProportional : 1;
	uint16 bitpadding_26_14 : 1;
	uint16 UsingRTCPDuration : 1;

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439DDDB0; }
}; //Size: 0x0028
assert_sizeof(AtomStateTransitionTarget, 0x28);

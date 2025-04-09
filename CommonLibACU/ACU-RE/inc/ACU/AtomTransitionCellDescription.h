#pragma once

#include "AtomNodeID.h"

class AtomStateMachineNode;

class AtomTransitionCellDescription
{
public:
	AtomStateMachineNode* stateMachineNode; //0x0000
	uint32 BlendTypeFromOrigin : 4; // Enum ACTBlendType
	uint32 BlendAlgoFromOrigin : 4; // Enum ACTBlendAlgo
	uint32 TranslationSourceFromOrigin : 2; // Enum ACTBlendDispSrcMode
	uint32 RotationSourceFromOrigin : 2; // Enum ACTBlendDispSrcMode
	uint32 ActorRef1SourceFromOrigin : 2; // Enum ACTBlendDispSrcMode
	uint32 BlendTypeToDestination : 4; // Enum ACTBlendType
	uint32 BlendAlgoToDestination : 4; // Enum ACTBlendAlgo
	uint32 TranslationSourceToDestination : 2; // Enum ACTBlendDispSrcMode
	uint32 RotationSourceToDestination : 2; // Enum ACTBlendDispSrcMode
	uint32 ActorRef1SourceToDestination : 2; // Enum ACTBlendDispSrcMode
	float BlendDurationFromOrigin; //0x000C
	float BlendDurationToDestination; //0x0010
	AtomNodeID AtomTaskID; //0x0014
	AtomNodeID OriginStateTaskID; //0x0024
	AtomNodeID TargetStateTaskID; //0x0034
	AtomNodeID TransitionFromOriginTaskID; //0x0044
	char pad_0054[4]; //0x0054
}; //Size: 0x0058
assert_sizeof(AtomTransitionCellDescription, 0x58);

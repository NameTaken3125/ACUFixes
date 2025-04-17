#pragma once

#include "AtomGraphNode.h"

/*
5 input ports.
Inputs[4] is "bool IsDisabled" maybe? Skips whole evaluation, it seems. See 1400EA23A
*/
class AtomAnimationRootNode : public AtomGraphNode
{
public:
	// @members
	uint32 SyncStatusOffset; //0x0050
	uint32 CurrentLengthOffset; //0x0054
	uint32 CurrentRatioOffset; //0x0058
	int32 SuggestedSyncType; //0x005C
	float MaxSyncEntryRatio; //0x0060
	int32 FirstEvaluationOffset; //0x0064
	uint32 FirstEvaluationRatioOffset; //0x0068
	uint32 ForceEntryRatioOffset; //0x006C
	uint8 IsLoop; //0x0070
	uint8 UseOverrideRatio; //0x0071
	uint8 GenerateEndOfAction; //0x0072
	char pad_0073[5]; //0x0073

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D7870; }
}; //Size: 0x0078
assert_sizeof(AtomAnimationRootNode, 0x78);

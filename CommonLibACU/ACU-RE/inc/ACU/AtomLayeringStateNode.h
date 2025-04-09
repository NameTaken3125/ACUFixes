#pragma once

#include "AtomStateNode.h"
#include "AtomNodeID.h"
#include "SharedPtr.h"

class AtomSelectBoneMaskDecisionNode;

class BoneWeight
{
public:
	// @members
	uint32 BoneID; //0x0000
	uint8 WeightF8; //0x0004
	char pad_0005[3]; //0x0005

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439DEC00; }
}; //Size: 0x0008
assert_sizeof(BoneWeight, 8);

class Animation;

class AtomLayeringInfo
{
public:
	// @members
	SmallArray<uint32> LayerMarkUpIDHierarchy; //0x0000
	SmallArray<uint32> UpperBodyMarkup; //0x000C
	SmallArray<uint32> RightHandMarkup; //0x0018
	SmallArray<uint32> LeftHandMarkup; //0x0024
	uint32 BlendType; //0x0030
	char pad_0034[4]; //0x0034
	AtomStateMachineNode* StateImplementation; //0x0038
	AtomSelectBoneMaskDecisionNode* RootDecisionMode; //0x0040
	SharedPtrNew<Animation>* AdditiveBaseAnimation; //0x0048
	uint32 CharacterResolution; //0x0050
	uint32 DynamicBoneLayerMarkup; //0x0054
	uint32 BlendBoneReferential; //0x0058
	int32 AdditiveBaseSyncPoint; //0x005C
	uint32 LayerId; //0x0060
	uint32 BoneWeightMapInitializedOffset_layeringIndex_mb; //0x0064
	uint8 IsDynamicBoneWeights; //0x0068
	uint8 NeedFeetIK; //0x0069
	SmallArray<BoneWeight> BoneWeights; //0x006A
	AtomNodeID AtomStateMachineNodeID; //0x0076
	char pad_0086[2]; //0x0086

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D9A10; }

}; //Size: 0x0088
assert_sizeof(AtomLayeringInfo, 0x88);

class AtomLayeringStateNode : public AtomStateNode
{
public:
	// @members
	SmallArray<AtomLayeringInfo> BlendLayers; //0x0080
	SmallArray<AtomGraphNode*> GraphNodes;
	char pad_0098[12]; //0x008C
	AtomNodeID AtomRootTaskID; //0x00A4
	char pad_00B4[4]; //0x00B4
	AtomLayeringInfo BaseLayer; //0x00B8
	uint32 LayerNodesInstanceSize; //0x0140
	uint32 RightFootIndexOffset; //0x0144
	uint32 LeftFootIndexOffset; //0x0148
	uint8 RequiresPerFramePreUpdate; //0x014C
	char pad_014D[3]; //0x014D

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D9960; }
}; //Size: 0x0150
assert_sizeof(AtomLayeringStateNode, 0x150);

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
	SmallArray<uint32> arr0; //0x0000
	char pad_000C[36]; //0x000C
	uint32 BlendType; //0x0030
	char pad_0034[4]; //0x0034
	AtomStateNode* stateNode38; //0x0038
	AtomSelectBoneMaskDecisionNode* boneMaskDecisionNode; //0x0040
	SharedPtrNew<Animation>* shared_Animation; //0x0048
	uint32 dword_50; //0x0050
	uint32 dword_54; //0x0054
	uint32 dword_58; //0x0058
	char pad_005C[4]; //0x005C
	uint32 LayerId; //0x0060
	char pad_0064[6]; //0x0064
	SmallArray<BoneWeight> BoneWeights; //0x006A
	AtomNodeID nodeID; //0x0076
	char pad_0086[2]; //0x0086

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D9A10; }

}; //Size: 0x0088
assert_sizeof(AtomLayeringInfo, 0x88);

class AtomLayeringStateNode : public AtomStateNode
{
public:
	// @members
	SmallArray<AtomLayeringInfo> layeringInfos; //0x0080
	SmallArray<AtomGraphNode*> someGraphNodesRelatedToExtraLayers;
	char pad_0098[12]; //0x008C
	AtomNodeID nodeID; //0x00A4
	char pad_00B4[4]; //0x00B4
	AtomLayeringInfo BaseLayer; //0x00B8
	char pad_0140[16]; //0x0140

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D9960; }
}; //Size: 0x0150
assert_sizeof(AtomLayeringStateNode, 0x150);

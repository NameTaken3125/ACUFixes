#pragma once


#include "AtomGraphNode.h"
#include "SharedPtr.h"

class Animation;

class AtomAnimationDataBaseNode : public AtomGraphNode
{
public:
	SharedPtrNew<Animation>* shared_Animation; //0x0050
	uint32 outputDataOffsetMinus10; //0x0058
	uint32 outputDataOffsetMinus6; //0x005C
	uint32 outputDataOffsetMinus2; //0x0060
	uint16 AnimationProviderID; //0x0064
	uint16 word_66; //0x0066
	uint16 word_68; //0x0068
	uint16 word_6A; //0x006A
	char pad_006C[4]; //0x006C
}; //Size: 0x0070
assert_sizeof(AtomAnimationDataBaseNode, 0x70);

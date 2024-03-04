#pragma once

#include "AtomGraphNode.h"

class AtomAnimationRootNode : public AtomGraphNode
{
public:
	// @members
	uint32 dword_50; //0x0050
	uint32 CurrentLengthOffset; //0x0054
	uint32 CurrentRatioOffset; //0x0058
	int32 int_5C; //0x005C
	float flt_60; //0x0060
	int32 DataOffsetBegin_mb; //0x0064
	uint32 dword_68; //0x0068
	uint32 dword_6C; //0x006C
	uint8 bool_70; //0x0070
	uint8 bool_71; //0x0071
	uint8 bool_72; //0x0072
	char pad_0073[5]; //0x0073

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D7870; }
}; //Size: 0x0078
assert_sizeof(AtomAnimationRootNode, 0x78);

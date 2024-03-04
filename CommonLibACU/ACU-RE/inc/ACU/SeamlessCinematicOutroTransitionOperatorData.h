#pragma once

#include "DominoOperatorData.h"

class SeamlessCinematicOutroTransitionOperatorData : public DominoOperatorData
{
public:
	class SharedPtr* shared_entity; //0x0060
	uint32 dword_68; //0x0068
	char pad_006C[12]; //0x006C
	float flt_78; //0x0078
	char pad_007C[12]; //0x007C
}; //Size: 0x0088
assert_sizeof(SeamlessCinematicOutroTransitionOperatorData, 0x88);

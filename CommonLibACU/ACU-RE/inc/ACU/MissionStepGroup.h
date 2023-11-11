#pragma once

#include "MissionStep.h"

class MissionStepGroup : public MissionStep
{
public:
	char pad_00A0[32]; //0x00A0
}; //Size: 0x00C0
assert_sizeof(MissionStepGroup, 0xC0);

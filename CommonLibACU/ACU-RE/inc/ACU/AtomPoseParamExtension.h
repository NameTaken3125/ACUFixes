#pragma once

#include "AtomStateExtension.h"

class AtomPoseParamExtension : public AtomStateExtension
{
public:
	uint16 PoseParamIndex; //0x0008
	char pad_000A[6]; //0x000A
}; //Size: 0x0010
assert_sizeof(AtomPoseParamExtension, 0x10);

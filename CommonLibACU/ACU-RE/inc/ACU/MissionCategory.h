#pragma once

#include "ManagedObject.h"


class MissionCategory : public ManagedObject
{
public:
	char pad_0010[8]; //0x0010
}; //Size: 0x0018
assert_sizeof(MissionCategory, 0x18);

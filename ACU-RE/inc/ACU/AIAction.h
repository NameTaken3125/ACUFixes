#pragma once

#include "ManagedObject.h"

class AIAction : public ManagedObject
{
public:
	char pad_0010[24]; //0x0010
	char* str_targetEntity; //0x0028
	char pad_0030[16]; //0x0030
}; //Size: 0x0040
assert_sizeof(AIAction, 0x40);

#pragma once

#include "ManagedObject.h"

class UIEventReaction : public Object
{
public:
	char pad_0008[296]; //0x0008
}; //Size: 0x0130
assert_sizeof(UIEventReaction, 0x130);

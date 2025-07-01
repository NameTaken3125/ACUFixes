#pragma once

#include "ManagedObject.h"

class FXProperty;

class FXElement : public Object
{
public:
	SmallArray<FXProperty> FXProperties; //0x0008
	float BeginTime; //0x0014
	float EndTime; //0x0018
	char pad_001C[4]; //0x001C
}; //Size: 0x0020
assert_sizeof(FXElement, 0x20);

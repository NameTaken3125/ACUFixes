#pragma once

#include "MandatoryUniverseComponent.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class TagRules;

class UniverseTagRules : public UniverseComponent
{
public:
	// @members
	SmallArray<SharedPtrNew<TagRules>*> Rules; //0x0010
	char pad_001C[4]; //0x001C

	// @helper_functions
	static UniverseTagRules* GetSingleton() { return *(UniverseTagRules**)0x1452173E8; }
}; //Size: 0x0020
assert_sizeof(UniverseTagRules, 0x20);

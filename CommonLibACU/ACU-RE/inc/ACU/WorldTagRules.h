#pragma once

#include "WorldComponent.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class TagRules;

class WorldTagRules : public WorldComponent
{
public:
	// @members
	SmallArray<SharedPtrNew<TagRules>*> Rules; //0x0020
	char pad_002C[4]; //0x002C

	// @helper_functions
	static WorldTagRules* GetSingleton();
}; //Size: 0x0030
assert_sizeof(WorldTagRules, 0x30);

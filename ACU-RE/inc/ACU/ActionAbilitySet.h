#pragma once

#include "AIAction.h"
#include "SharedPtr.h"

class AssassinAbilitySet;

class ActionAbilitySet : public AIAction
{
public:
	SharedPtrNew<AssassinAbilitySet>* shared_AbilitySet; //0x0040
	char pad_0048[8]; //0x0048
}; //Size: 0x0050
assert_sizeof(ActionAbilitySet, 0x50);

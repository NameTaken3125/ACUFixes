#pragma once

#include "AtomStateNode.h"

class AtomNullStateNode : public AtomStateNode
{
public:
	// @members

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439DD9F0; }
}; //Size: 0x0080
assert_sizeof(AtomNullStateNode, 0x80);

#pragma once

#include "AtomAnimationDataBaseNode.h"

class AtomAnimationDataNode : public AtomAnimationDataBaseNode
{
public:
	// @members

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D7660; }
}; //Size: 0x0070
assert_sizeof(AtomAnimationDataNode, 0x70);

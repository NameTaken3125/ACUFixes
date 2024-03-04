#pragma once

#include "basic_types.h"
#include "SmallArray.h"

class AtomTransitionCellDescription;

class AtomCustomTransitionSystem
{
public:
	SmallArray<AtomTransitionCellDescription> transitionCells; //0x0000
}; //Size: 0x000C
assert_sizeof(AtomCustomTransitionSystem, 0xC);

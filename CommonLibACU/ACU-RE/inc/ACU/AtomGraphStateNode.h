#pragma once

#include "AtomStateNode.h"

class AtomGraphNode;

class AtomGraphStateNode : public AtomStateNode
{
public:
	SmallArray<AtomGraphNode*> graphNodes; //0x0080
	char pad_008C[36]; //0x008C
}; //Size: 0x00B0

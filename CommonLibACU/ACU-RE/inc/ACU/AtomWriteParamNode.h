#pragma once

#include "AtomGraphNode.h"

class AtomWriteParamNode : public AtomGraphNode
{
public:
	uint32 DataType; //0x0050
	uint32 ParameterID; //0x0054
}; //Size: 0x0058
assert_sizeof(AtomWriteParamNode, 0x58);

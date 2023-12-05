#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "AtomNodeID.h"


class AtomGraphNode;

class AtomInputPort
{
public:
	uint32 type; //0x0000
	char pad_0004[12]; //0x0004
	class AtomGraphNode** ppNodeIfTypeEq9; //0x0010
	char pad_0018[8]; //0x0018
}; //Size: 0x0020
assert_sizeof(AtomInputPort, 0x20);

class AtomOutputPort
{
public:
	AtomGraphNode* graphNode; //0x0000
	uint32 nodeIndex_mb; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
assert_sizeof(AtomOutputPort, 0x10);

class AtomGraphNode : public Object
{
public:
	SmallArray<AtomInputPort> InputPorts; //0x0008
	char pad_0014[4]; //0x0014
	SmallArray<AtomOutputPort> OutputPorts; //0x0018
	char pad_0024[12]; //0x0024
	uint32 nodeIndex_mb; //0x0030
	char pad_0034[4]; //0x0034
	AtomNodeID nodeID; //0x0038
	uint16 word_48; //0x0048
	char pad_004A[6]; //0x004A
}; //Size: 0x0050
assert_sizeof(AtomGraphNode, 0x50);

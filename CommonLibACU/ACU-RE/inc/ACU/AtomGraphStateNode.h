#pragma once

#include "AtomStateNode.h"
#include "SmallArray.h"

class AtomGraphNode;

class AtomGraphStateNode : public AtomStateNode
{
public:
	// @members
	SmallArray<AtomGraphNode*> Nodes; //0x0080
	SmallArray<uint32> RTCPIndices; //0x008C
	uint32 ExternalID; //0x0098
	uint32 SyncedEntitySelfIndexOffset; //0x009C
	uint16 NumberOfAnimationProviders; //0x00A0
	uint16 NumberOfNonLinkedEntries; //0x00A2
	uint16 NumberOfDisplacementResults; //0x00A4
	uint16 NumberOfTagSynchronizationEntries; //0x00A6
	uint16 NumberOfPreUpdateEntries_numNodes_mb; //0x00A8
	uint8 NeedGlobalPose; //0x00AA
	uint8 RequiresPerFramePreUpdate; //0x00AB
	char pad_00AC[4]; //0x00AC

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439DD940; }
}; //Size: 0x00B0
assert_sizeof(AtomGraphStateNode, 0xB0);

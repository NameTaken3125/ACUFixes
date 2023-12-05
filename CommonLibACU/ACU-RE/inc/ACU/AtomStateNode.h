#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "AtomNodeID.h"

class AtomStateTransitionTarget;

class AtomStateNode_Base8
{
public:
    char pad_0000[8]; //0x0000
    SmallArray<AtomStateTransitionTarget*> Transitions; //0x0008
    SmallArray<AtomNodeID> nodeIDs; //0x0014
    char pad_0020[34]; //0x0020
    uint16 word_42; //0x0042
    char pad_0044[4]; //0x0044
}; //Size: 0x0048

class AtomStateNode : public Object
{
public:
    AtomStateNode_Base8 base8; //0x0008
    AtomStateNode* parentNode_mb; //0x0050
    char pad_0058[16]; //0x0058
    AtomNodeID nodeID; //0x0068
    char pad_0078[8]; //0x0078
}; //Size: 0x0080
assert_sizeof(AtomStateNode, 0x80);

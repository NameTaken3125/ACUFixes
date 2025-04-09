#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "AtomNodeID.h"

class AtomStateTransitionTarget;
class AtomStateExtension;

class AtomStateNode_Base8
{
public:
    char pad_0000[8]; //0x0000
    SmallArray<AtomStateTransitionTarget*> Transitions; //0x0008
    SmallArray<AtomNodeID> AtomRefParentID; //0x0014
    SmallArray<uint16> ArrivingTransitionIndices; //0x0020
    SmallArray<uint16> DepartingTransitionIndices; //0x002C
    SmallArray<AtomStateExtension*> Extensions; //0x0038
    char pad_0044[4]; //0x0044
}; //Size: 0x0048

class AtomStateNode : public Object
{
public:
    AtomStateNode_Base8 base8; //0x0008
    AtomStateNode* parentNode_mb; //0x0050
    SmallArray<uint32> AnimMarkups; //0x0058
    uint32 InstanceDataSize; //0x0064
    class AtomNodeID AtomTaskID; //0x0068
    uint32 RuntimeStateId; //0x0078
    char pad_007C[4]; //0x007C
}; //Size: 0x0080
assert_sizeof(AtomStateNode, 0x80);

#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class AtomRawFile : public ManagedObject
{
public:
}; //Size: 0x0010
assert_sizeof(AtomRawFile, 0x10);

class AtomGraph;

class AtomStateDescriptor : public Object
{
public:
    char pad_0008[24]; //0x0008
    AtomGraph* graph; //0x0020
}; //Size: 0x0028
assert_sizeof(AtomStateDescriptor, 0x28);

class ExternalIdMap
{
public:
    char pad_0000[24]; //0x0000
}; //Size: 0x0018
assert_sizeof(ExternalIdMap, 0x18);

class AtomStateMachineNode;
class AtomGraphStateNode;
class AtomNullStateNode;
class AtomRTCPDescriptor;
class AtomCustomTransitionSystem;

class AtomAnimComponent_D0_678;

class AtomGraph : public AtomRawFile
{
public:
    AtomStateMachineNode* stateMachineNode; //0x0010
    AtomGraphStateNode* graphNode; //0x0018
    AtomNullStateNode* AtomNullStateNode_; //0x0020
    SmallArray<AtomRTCPDescriptor> rtcpDescriptors; //0x0028
    char pad_0034[4]; //0x0034
    AtomStateDescriptor stateDescriptor; //0x0038
    AtomCustomTransitionSystem* AtomCustomTransitionSystem_; //0x0060
    char pad_0068[12]; //0x0068
    ExternalIdMap ExternalIdMap_; //0x0074
    char pad_008C[4]; //0x008C
    AtomAnimComponent_D0_678* p90; //0x0090
    char pad_0098[8]; //0x0098
}; //Size: 0x00A0
assert_sizeof(AtomGraph, 0xA0);

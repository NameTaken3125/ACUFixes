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

#include "ACUHashmap.h"
class AtomGraph_RTCP
{
public:
    char pad_0000[32]; //0x0000
    SmallArray<byte> graphVarsBuffer; //0x0020
    char pad_002C[164]; //0x002C
    SmallArray<uint32> graphVarsOffsets; //0x00D0
    SmallArray<uint32> graphVarHashes; //0x00DC
    ACUHashmap<uint32, uint32> atomGraphVarsHashmap; //0x00E8
    AtomGraph* atomGraph; //0x00F0
    char pad_00F8[8]; //0x00F8
}; //Size: 0x0110
assert_offsetof(AtomGraph_RTCP, graphVarsOffsets, 0xD0);
assert_offsetof(AtomGraph_RTCP, atomGraphVarsHashmap, 0xE8);
assert_sizeof(AtomGraph_RTCP, 0x100);


class AtomGraph_98
{
public:
    char pad_0000[24]; //0x0000
    uint64 hashmapOfSkeletonCombinations_mb; //0x0018
    char pad_0020[48]; //0x0020
}; //Size: 0x0050

class AtomGraph : public AtomRawFile
{
public:
    // @members
    AtomStateMachineNode* RootStateMachine; //0x0010
    AtomGraphStateNode* PostProcessGraph; //0x0018
    AtomNullStateNode* ErrorState; //0x0020
    SmallArray<AtomRTCPDescriptor> rtcpDescriptors; //0x0028
    char pad_0034[4]; //0x0034
    AtomStateDescriptor PoseParamStateDescriptor; //0x0038
    AtomCustomTransitionSystem* CustomTransitionSystem; //0x0060
    uint32 RTCPDataSize; //0x0068
    uint32 FirstEntityRefRTCPIndex; //0x006C
    uint32 LifetimeInstanceDataSize; //0x0070
    ExternalIdMap ExternalIdMap_; //0x0074
    char pad_008C[4]; //0x008C
    AtomGraph_RTCP* rtcp; //0x0090
    AtomGraph_98* p98; //0x0098

    // @members
    static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D88E0; }
}; //Size: 0x00A0
assert_sizeof(AtomGraph, 0xA0);

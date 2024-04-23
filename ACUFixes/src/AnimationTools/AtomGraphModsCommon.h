#pragma once

#include "ACU_DefineNativeFunction.h"

#include "ACU/ACUHashmap.h"
#include "ACU/AtomGraph.h"

#include "ACU/AtomStateNode.h"
#include "ACU/AtomNullStateNode.h"
#include "ACU/AtomGraphStateNode.h"
#include "ACU/AtomStateMachineNode.h"
#include "ACU/AtomLayeringStateNode.h"
#include "ACU/AtomStateTransitionTarget.h"

#include "ACU/AtomGraphNode.h"
#include "ACU/AtomAnimationDataNode.h"
#include "ACU/AtomAnimationRootNode.h"

#include "ACU/AtomCondition.h"
#include "ACU/AtomConditionExpression.h"

#include "ACU_SharedPtrs.h"
#include "Experimental_StrongPtr.h"

#include "ACUAllocs.h"

#include "AnimationTools/UsefulHandles.h"


DEFINE_GAME_FUNCTION(HashmapGeneric__AddElement, 0x14277B2A0, void*, __fastcall, (ACUHashmapGeneric* p_hashmap, unsigned __int64 p_key, __int64 p_errorOut_mb));
// Returns ptr to element, or `-1` if fails for some reason, I guess?
void* ACUHashmapGeneric_AddElement(ACUHashmapGeneric* hashmap, uint64 key)
{
    return HashmapGeneric__AddElement(hashmap, key, 0);
}
template<typename Key_t, typename Value_t>
void ACUHashmap_Set(ACUHashmap<Key_t, Value_t>& hashmap, Key_t key, Value_t value)
{
    *(Value_t*)ACUHashmapGeneric_AddElement(&hashmap.m_map, key) = value;
}

void* AddMyNewRTCPVariable_generic(AtomGraph& atomGraph, uint8 varAlignment, uint8 varSize, uint32 varnameHash)
{
    // Kind of taken from `AtomGraph_90::Init()` at 0x140123630.
    AtomGraph_RTCP* rtcpCached = atomGraph.rtcp;

    const uint16 oldRTCPDataSize = rtcpCached->graphVarsBuffer.size;
    const uint16 roundUpToRequiredAlignment = (oldRTCPDataSize / varAlignment + (oldRTCPDataSize % varAlignment > 0)) * varAlignment;
    const uint16 positionInVarsBuffer = roundUpToRequiredAlignment;
    const uint16 newRTCPDataSize = positionInVarsBuffer + varSize;

    uint16 newNumHashes = rtcpCached->graphVarHashes.size + 1;

    const uint16 oldNumNormalVars = rtcpCached->graphVarsOffsets.size;
    const uint16 newVarIdx = oldNumNormalVars;
    uint32 numRtcpDescriptorsWithoutFixedOffset = rtcpCached->graphVarHashes.size - oldNumNormalVars;
    uint16 newNumOffsets = newNumHashes - numRtcpDescriptorsWithoutFixedOffset;
    atomGraph.numNonentityrefRTCPVars = newNumOffsets;

    SmallArrayReserve(rtcpCached->graphVarsBuffer, newRTCPDataSize);
    rtcpCached->graphVarsBuffer.size = newRTCPDataSize;
    SmallArrayInsert(rtcpCached->graphVarsOffsets, (uint32)positionInVarsBuffer, newVarIdx);
    SmallArrayInsert(rtcpCached->graphVarHashes, varnameHash, newVarIdx);
    ACUHashmap_Set(rtcpCached->atomGraphVarsHashmap, varnameHash, (uint32)newVarIdx);
    // entityref SyncParticipant1; // 0x324db80/52747136
    // entityref SyncSubject; // 0xe35d816d/3814556013
    (*rtcpCached->atomGraphVarsHashmap.Get(0x324db80))++;
    (*rtcpCached->atomGraphVarsHashmap.Get(0xe35d816d))++;
    return (void*)&rtcpCached->graphVarsBuffer[positionInVarsBuffer];
}
class AtomGraphPatchesDatabase
{
public:
    void AddRTCPVariable(AtomGraph& graph, const MyNewerRTCPVariable& newVarDescriptor);
    std::map<AtomGraph*, uint32> m_numAddedNewVarsByGraph;
};
AtomGraphPatchesDatabase g_GraphPatches;
void AtomGraphPatchesDatabase::AddRTCPVariable(AtomGraph& atomGraph, const MyNewerRTCPVariable& newVarDescriptor)
{
    AtomGraph_RTCP* rtcpCached = atomGraph.rtcp;
    if (uint32* pAlreadyPresentVarIdx = rtcpCached->atomGraphVarsHashmap.Get(newVarDescriptor.varnameHash))
    {
        return;
    }
    switch (newVarDescriptor.vartype)
    {
    case RTCPVariableType::Bool:
        *(bool*)AddMyNewRTCPVariable_generic(atomGraph, 1, 1, newVarDescriptor.varnameHash) = newVarDescriptor.bool_;
        break;
    case RTCPVariableType::Float:
        *(float*)AddMyNewRTCPVariable_generic(atomGraph, 4, 4, newVarDescriptor.varnameHash) = newVarDescriptor.float_;
        break;
    case RTCPVariableType::Int:
        *(int*)AddMyNewRTCPVariable_generic(atomGraph, 4, 4, newVarDescriptor.varnameHash) = newVarDescriptor.int_;
        break;
    default:
        break;
    }
    m_numAddedNewVarsByGraph[&atomGraph]++;
}
//uint32 AdjustGraphvarIndexForEntityrefVars(uint32 graphVarIdx, AtomGraph& graph)
//{
//    return graphVarIdx + 1;
//}
uint32 AdjustGraphvarIndexForEntityrefVars(uint32 graphVarIdx, AtomGraph& graph)
{
    return graphVarIdx + g_GraphPatches.m_numAddedNewVarsByGraph[&graph];
}
void AddMyNewerRTCPVariable(AtomGraph& atomGraph, const MyNewerRTCPVariable& newVarDescriptor)
{
    g_GraphPatches.AddRTCPVariable(atomGraph, newVarDescriptor);
}
AtomOutputPort* CreateOutputPort(AtomGraphNode& graphNode)
{
    AtomOutputPort* defaultOutputPort = SmallArray_GameType_Append(graphNode.OutputPorts);
    defaultOutputPort->graphNode = &graphNode;
    return defaultOutputPort;
}
union InputPortPossibleValue_t
{
    AtomOutputPort* outport;
    int integer;
    float flt;
    InputPortPossibleValue_t(int integer) : integer(integer) {}
    InputPortPossibleValue_t(float flt) : flt(flt) {}
};
AtomInputPort* CreateInputPort(AtomGraphNode& graphNode, uint32 bindingType, InputPortPossibleValue_t value)
{
    AtomInputPort* newInport = SmallArray_GameType_Append(graphNode.InputPorts);
    newInport->BindingType = bindingType;
    (InputPortPossibleValue_t&)newInport->outputPortIfTypeEq9_graphVarIdxIfEq0 = value;
    return newInport;
}

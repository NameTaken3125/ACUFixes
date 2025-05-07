#include "pch.h"

#include "AnimationGraphEvaluationPatches.h"
#include "Common_Plugins/AnimationGraphMods/RTCPVariableDescriptor.h"
#include "Common_Plugins/ACUAllocs.h"

#include "ACU_DefineNativeFunction.h"
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
    atomGraph.FirstEntityRefRTCPIndex = newNumOffsets;

    SmallArrayReserve(rtcpCached->graphVarsBuffer, newRTCPDataSize);
    rtcpCached->graphVarsBuffer.size = newRTCPDataSize;
    SmallArrayInsert(rtcpCached->graphVarsOffsets, (uint32)positionInVarsBuffer, newVarIdx);
    SmallArrayInsert(rtcpCached->graphVarHashes, varnameHash, newVarIdx);
    ACUHashmap_Set(rtcpCached->atomGraphVarsHashmap, varnameHash, (uint32)newVarIdx);
    // The graph has just two `entityref`-type RTCP variables:
    // entityref SyncParticipant1; // 0x324db80/52747136    // index==539
    // entityref SyncSubject; // 0xe35d816d/3814556013      // index==540
    // These two get special treatment.
    // See comments at `AnimationGraphEvaluationPatches::AnimationGraphEvaluationPatches()`.
    (*rtcpCached->atomGraphVarsHashmap.Get(0x324db80))++;
    (*rtcpCached->atomGraphVarsHashmap.Get(0xe35d816d))++;
    return (void*)&rtcpCached->graphVarsBuffer[positionInVarsBuffer];
}
class AtomGraphPatchesDatabase
{
public:
    void AddRTCPVariable(AtomGraph& graph, const RTCPVariableDescriptor& newVarDescriptor);
    std::map<AtomGraph*, uint32> m_numAddedNewVarsByGraph;
};
AtomGraphPatchesDatabase g_GraphPatches;
void AtomGraphPatchesDatabase::AddRTCPVariable(AtomGraph& atomGraph, const RTCPVariableDescriptor& newVarDescriptor)
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
static uint32 AdjustGraphvarIndexForEntityrefVars(uint32 graphVarIdx, AtomGraph& graph)
{
    return graphVarIdx + g_GraphPatches.m_numAddedNewVarsByGraph[&graph];
}
void AddNewRTCPVariableIfNotPresent_impl(AtomGraph& atomGraph, const RTCPVariableDescriptor& newVarDescriptor)
{
    g_GraphPatches.AddRTCPVariable(atomGraph, newVarDescriptor);
}


void DatabaseOfCustomReactionsToAnimationSignals::RegisterSignal(SignalID_t signalInt)
{
    m_AppendedNewSignalReceiversInHumanStates.insert(signalInt);
}
void DatabaseOfCustomReactionsToAnimationSignals::RegisterCustomReaction(CustomReactionToAnimationSignals& newCustomReaction)
{
    m_CustomReactions.push_back(&newCustomReaction);
}
void DatabaseOfCustomReactionsToAnimationSignals::UnregisterCustomReaction(CustomReactionToAnimationSignals& reactionToRemove)
{
    m_CustomReactions.erase(std::remove(m_CustomReactions.begin(), m_CustomReactions.end(), &reactionToRemove), m_CustomReactions.end());
}
void RegisterCustomReaction_impl(CustomReactionToAnimationSignals& newCustomReaction)
{
    DatabaseOfCustomReactionsToAnimationSignals::GetSingleton().RegisterCustomReaction(newCustomReaction);
}
void UnregisterCustomReaction_impl(CustomReactionToAnimationSignals& reactionToRemove)
{
    DatabaseOfCustomReactionsToAnimationSignals::GetSingleton().UnregisterCustomReaction(reactionToRemove);
}
void RegisterSignal_impl(SignalID_t signalInt)
{
    DatabaseOfCustomReactionsToAnimationSignals::GetSingleton().RegisterSignal(signalInt);
}


void DatabaseOfCustomReactionsToAnimationSignals::WhenSignalChangeDispatched(HumanStatesHolder* receivingEntityHumanStates, SignalID_t signalInt, bool isSignalOn)
{
    for (CustomReactionToAnimationSignals* customReaction : m_CustomReactions)
    {
        customReaction->OnSignalChangeDispatched(receivingEntityHumanStates, signalInt, isSignalOn);
    }
}
void DatabaseOfCustomReactionsToAnimationSignals::Hook_WhenInitializingArrayOfIntegerSignalReceivers(ManagerOfAnimationSignalsReceivers& integerSignalReceiversManager)
{
    for (SignalID_t signalInt : m_AppendedNewSignalReceiversInHumanStates)
    {
        SmallArrayAppend(integerSignalReceiversManager.integerSignalReceivers, UsedDuringQuickshot_SignalReceiver(signalInt));
        UsedDuringQuickshot_SignalReceiver& signalReceiver = integerSignalReceiversManager.integerSignalReceivers[integerSignalReceiversManager.integerSignalReceivers.size - 1];
        signalReceiver.numListenersToThisSignal++;
        // "quickdrop" signal has `byte_C == 1`, while "unholster" and "shot fired" have it at `0`. idk.
        signalReceiver.byte_C = 1;
        signalReceiver.dword_8 = 0xFFFFFFFF;
    }
}
void WhenInitializingArrayOfIntegerSignalReceivers_FinishedBuildingDefaultSignalReceivers_AddCustomSignals(ManagerOfAnimationSignalsReceivers& integerSignalReceiversManager)
{
    DatabaseOfCustomReactionsToAnimationSignals::GetSingleton().Hook_WhenInitializingArrayOfIntegerSignalReceivers(integerSignalReceiversManager);
}
void WhenInitializingArrayOfIntegerSignalReceivers_FinishedBuildingDefaultSignalReceivers_AddCustomSignals(AllRegisters* params)
{
    ManagerOfAnimationSignalsReceivers* integerSignalReceiversManager = (ManagerOfAnimationSignalsReceivers*)params->rsi_;
    DatabaseOfCustomReactionsToAnimationSignals::GetSingleton().Hook_WhenInitializingArrayOfIntegerSignalReceivers(*integerSignalReceiversManager);
}
void WhenEnabledSignalReceiverHasReceivedAChange_DispatchToCustomSubscribers(AllRegisters* params)
{
    auto* humanStates = (HumanStatesHolder*)params->rcx_;
    const uint16 idxOfRegisteredIntegerSignal = (uint16&)params->rdx_;
    const bool isSignalOn = (bool&)params->r8_;
    UsedDuringQuickshot_SignalReceiver& integerSignalReceiver = humanStates->integerSignalReceiversManager->integerSignalReceivers[idxOfRegisteredIntegerSignal];
    SignalID_t signalInt = integerSignalReceiver.animationSignalValue;
    DatabaseOfCustomReactionsToAnimationSignals::GetSingleton().WhenSignalChangeDispatched(
        humanStates,
        signalInt,
        isSignalOn
    );
}



void WhenResolvingEntityrefGraphVarIndex_2_adjustIndex(AllRegisters* params)
{
    uint32& graphvarIdx = (uint32&)params->rcx_;
    GraphEvaluation* graphEval = (GraphEvaluation*)params->rdx_;
    AtomGraph* graph = graphEval->atomGraph;
    graphvarIdx = AdjustGraphvarIndexForEntityrefVars(graphvarIdx, *graph);
}
void WhenResolvingEntityrefGraphVarIndex_3_adjustIndex(AllRegisters* params)
{
    uint32& graphvarIdx = (uint32&)params->rdx_;
    AtomGraph_RTCP* rtcp = (AtomGraph_RTCP*)params->rcx_;
    AtomGraph* graph = rtcp->atomGraph;
    graphvarIdx = AdjustGraphvarIndexForEntityrefVars(graphvarIdx, *graph);
}
AnimationGraphEvaluationPatches::AnimationGraphEvaluationPatches()
{
    // The graph has just two `entityref`-type RTCP variables:
    // entityref SyncParticipant1; // 0x324db80/52747136    // index==539
    // entityref SyncSubject; // 0xe35d816d/3814556013      // index==540
    // These two get special treatment.
    // Basically, the `entityref` variables are supposed to be the last ones,
    // supposed to come after all the "normal" RTCP variables.
    // This means that whenever I want to add a new "normal" RTCP variable, I need to _sort_of_ insert
    // them between the "normal" and the "entityref" variables.
    // Whenever some node of the `AtomGraph` reads the value of an RTCP variable, it does so directly by index.
    // On the other hand, whenever the "game code" side reads or writes the value of an RTCP values,
    // it does so by looking up the variable by the hash of its name.
    // So whenever I insert a new "normal" RTCP variable, I need to increase the indices of the `entityref` variables
    // by 1, but patch the "animation system" in such a way that it can correctly look up
    // the `entityref` variables by their old indices.
    uintptr_t whenResolvingEntityrefGraphVarIndex_2 = 0x1400B5060;
    PresetScript_CCodeInTheMiddle(whenResolvingEntityrefGraphVarIndex_2, 5,
        WhenResolvingEntityrefGraphVarIndex_2_adjustIndex, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenResolvingEntityrefGraphVarIndex_3 = 0x1400A29A0;
    PresetScript_CCodeInTheMiddle(whenResolvingEntityrefGraphVarIndex_3, 5,
        WhenResolvingEntityrefGraphVarIndex_3_adjustIndex, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);




    uintptr_t whenEnabledSignalReceiverHasReceivedAChange = 0x141942CB0;
    PresetScript_CCodeInTheMiddle(whenEnabledSignalReceiverHasReceivedAChange, 5,
        WhenEnabledSignalReceiverHasReceivedAChange_DispatchToCustomSubscribers, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenFinishedBuildingDefaultSignalReceivers = 0x1401C0ADA;
    PresetScript_CCodeInTheMiddle(whenFinishedBuildingDefaultSignalReceivers, 8,
        WhenInitializingArrayOfIntegerSignalReceivers_FinishedBuildingDefaultSignalReceivers_AddCustomSignals, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

}

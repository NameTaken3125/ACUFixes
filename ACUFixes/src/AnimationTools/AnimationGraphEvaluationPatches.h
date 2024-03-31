#pragma once

#include "ACU/basic_types.h"
#include "ACU/AtomAnimComponent.h"
#include "ACU/AtomGraph.h"
#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct AnimationGraphEvaluationPatches : AutoAssemblerCodeHolder_Base
{
    AnimationGraphEvaluationPatches();
    virtual void OnBeforeActivate() override;
};
uint32 AdjustGraphvarIndexForEntityrefVars(uint32 graphVarIdx, AtomGraph& graph);
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
//uint16 ResolveEntityrefVarIndex(uint32 entityrefGraphvarIndexToResolve);
//void WhenResolvingEntityrefGraphVarIndex_afterLookupByHash_AdjustTheIndex(AllRegisters* params)
//{
//    GraphEvaluation* graphEvaluation = (GraphEvaluation*)params->rbx_;
//    params->GetRAX() = (uint64)graphEvaluation->stru400.arrEntityrefGraphvars.arr;
//
//    const uint32 entityrefGraphvarIndexToResolve = (uint32&)params->rsi_;
//    const uint16 resolvedEntityrefVarIndex = ResolveEntityrefVarIndex(entityrefGraphvarIndexToResolve);
//    params->rsi_ = resolvedEntityrefVarIndex;
//}
void LetsDoSomePlayerGraphMods();
void AnimationGraphEvaluationPatches::OnBeforeActivate()
{
    LetsDoSomePlayerGraphMods();
}

using SignalID_t = uint32;
constexpr SignalID_t signal_RangedWeaponShotFired = 0x800023;
constexpr SignalID_t signal_RangedWeaponUnholster = 0x80005A;
constexpr SignalID_t signal_EnteredHidespotCloset = 0x800024;
constexpr SignalID_t signal_HoodPutOn = 0x800177;
constexpr SignalID_t signal_HoodTakeOff = 0x800077;
class CustomReactionToAnimationSignals
{
public:
    virtual ~CustomReactionToAnimationSignals() {}
    virtual void OnSignalChangeDispatched(HumanStatesHolder* receivingEntityHumanStates, SignalID_t signalInt, bool isSignalOn) = 0;
};
void RemovableHood_ReactToAnimationSignal(bool truePutOnFalseTakeOff);
class ReactToAnimationHoodOnOff : public CustomReactionToAnimationSignals
{
public:
    virtual void OnSignalChangeDispatched(HumanStatesHolder* receivingEntityHumanStates, SignalID_t signalInt, bool isSignalOn) override
    {
        if (signalInt == signal_HoodPutOn)
        {
            if (isSignalOn)
            {
                RemovableHood_ReactToAnimationSignal(true);
            }
        }
        else if (signalInt == signal_HoodTakeOff)
        {
            if (isSignalOn)
            {
                RemovableHood_ReactToAnimationSignal(false);
            }
        }
    }
};
class DatabaseOfCustomReactionsToAnimationSignals
{
public:
    DatabaseOfCustomReactionsToAnimationSignals()
    {
        static ReactToAnimationHoodOnOff s;
        this->RegisterCustomReaction(s);
        this->RegisterSignal(signal_HoodPutOn);
        this->RegisterSignal(signal_HoodTakeOff);
    }
public:
    void RegisterCustomReaction(CustomReactionToAnimationSignals& newCustomReceiver);
    void RegisterSignal(SignalID_t signalInt);
    void WhenSignalChangeDispatched(HumanStatesHolder* receivingEntityHumanStates, SignalID_t signalInt, bool isSignalOn);
public:
    void Hook_WhenInitializingArrayOfIntegerSignalReceivers(UsedDuringQuickshot& integerSignalReceiversManager);
private:
    std::vector<CustomReactionToAnimationSignals*> m_CustomReactions;
public:
    std::set<SignalID_t> m_AppendedNewSignalReceiversInHumanStates;
};
void DatabaseOfCustomReactionsToAnimationSignals::RegisterSignal(SignalID_t signalInt)
{
    m_AppendedNewSignalReceiversInHumanStates.insert(signalInt);
}
void DatabaseOfCustomReactionsToAnimationSignals::RegisterCustomReaction(CustomReactionToAnimationSignals& newCustomReaction)
{
    m_CustomReactions.push_back(&newCustomReaction);
}
DatabaseOfCustomReactionsToAnimationSignals g_DatabaseOfCustomReactionsToAnimationSignals;
#include "ACUAllocs.h"
void DatabaseOfCustomReactionsToAnimationSignals::WhenSignalChangeDispatched(HumanStatesHolder* receivingEntityHumanStates, SignalID_t signalInt, bool isSignalOn)
{
    for (CustomReactionToAnimationSignals* customReaction : m_CustomReactions)
    {
        customReaction->OnSignalChangeDispatched(receivingEntityHumanStates, signalInt, isSignalOn);
    }
}
void DatabaseOfCustomReactionsToAnimationSignals::Hook_WhenInitializingArrayOfIntegerSignalReceivers(UsedDuringQuickshot& integerSignalReceiversManager)
{
    for (SignalID_t signalInt : m_AppendedNewSignalReceiversInHumanStates)
    {
        SmallArrayAppend(integerSignalReceiversManager.animEvents_mb, UsedDuringQuickshot_40(signalInt));
        UsedDuringQuickshot_40& signalReceiver = integerSignalReceiversManager.animEvents_mb[integerSignalReceiversManager.animEvents_mb.size - 1];
        signalReceiver.dword_10_isPlaying_mb++;
        // "quickdrop" signal has `byte_C == 1`, while "unholster" and "shot fired" have it at `0`. idk.
        signalReceiver.byte_C = 1;
        signalReceiver.dword_8 = 0xFFFFFFFF;
    }
}
void WhenInitializingArrayOfIntegerSignalReceivers_FinishedBuildingDefaultSignalReceivers_AddCustomSignals(UsedDuringQuickshot& integerSignalReceiversManager)
{
    g_DatabaseOfCustomReactionsToAnimationSignals.Hook_WhenInitializingArrayOfIntegerSignalReceivers(integerSignalReceiversManager);
}
void WhenInitializingArrayOfIntegerSignalReceivers_FinishedBuildingDefaultSignalReceivers_AddCustomSignals(AllRegisters* params)
{
    UsedDuringQuickshot* integerSignalReceiversManager = (UsedDuringQuickshot*)params->rsi_;
    g_DatabaseOfCustomReactionsToAnimationSignals.Hook_WhenInitializingArrayOfIntegerSignalReceivers(*integerSignalReceiversManager);
}
void WhenEnabledSignalReceiverHasReceivedAChange_DispatchToCustomSubscribers(AllRegisters* params)
{
    auto* humanStates = (HumanStatesHolder*)params->rcx_;
    const uint16 idxOfRegisteredIntegerSignal = (uint16&)params->rdx_;
    const bool isSignalOn = (bool&)params->r8_;
    UsedDuringQuickshot_40& integerSignalReceiver = humanStates->hasAnimationEventsData_mb->animEvents_mb[idxOfRegisteredIntegerSignal];
    SignalID_t signalInt = integerSignalReceiver.dword_0;
    g_DatabaseOfCustomReactionsToAnimationSignals.WhenSignalChangeDispatched(
        humanStates,
        signalInt,
        isSignalOn
        );
}

AnimationGraphEvaluationPatches::AnimationGraphEvaluationPatches()
{
    //uintptr_t whenResolvingEntityrefGraphVarIndex_afterLookupByHash = 0x1400F1B29;
    //PresetScript_CCodeInTheMiddle(whenResolvingEntityrefGraphVarIndex_afterLookupByHash, 10,
    //    WhenResolvingEntityrefGraphVarIndex_afterLookupByHash_AdjustTheIndex, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);

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

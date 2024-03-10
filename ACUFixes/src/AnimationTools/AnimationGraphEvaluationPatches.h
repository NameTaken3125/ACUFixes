#pragma once

#include "ACU/basic_types.h"
#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct AnimationGraphEvaluationPatches : AutoAssemblerCodeHolder_Base
{
    AnimationGraphEvaluationPatches();
    virtual void OnBeforeActivate() override;
};
uint32 AdjustGraphvarIndexForEntityrefVars(uint32 graphVarIdx)
{
    return graphVarIdx + 1;
}
void WhenResolvingEntityrefGraphVarIndex_2_adjustIndex(AllRegisters* params)
{
    uint32& graphvarIdx = (uint32&)params->rcx_;
    graphvarIdx = AdjustGraphvarIndexForEntityrefVars(graphvarIdx);
}
void WhenResolvingEntityrefGraphVarIndex_3_adjustIndex(AllRegisters* params)
{
    uint32& graphvarIdx = (uint32&)params->rdx_;
    graphvarIdx = AdjustGraphvarIndexForEntityrefVars(graphvarIdx);
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
}
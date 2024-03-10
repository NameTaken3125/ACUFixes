#include "pch.h"

#include "AnimationTools/AnimGraphMods.h"
MyNewRTCPVariable g_newGraphVar;

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
    // TODO: Insert, not append.
    atomGraph.numRtcpDescriptors_6C = newNumOffsets;

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
void AddMyNewRTCPVariable_float(AtomGraph& atomGraph, uint32 varnameHash, float initialValue)
{
    *(float*)AddMyNewRTCPVariable_generic(atomGraph, 4, 4, varnameHash) = initialValue;
}
void AddMyNewRTCPVariable_bool(AtomGraph& atomGraph, uint32 varnameHash, bool initialValue)
{
    *(bool*)AddMyNewRTCPVariable_generic(atomGraph, 1, 1, varnameHash) = initialValue;
}
void AddMyNewRTCPVariable(AtomGraph& atomGraph)
{
    AddMyNewRTCPVariable_bool(atomGraph, g_newGraphVar.varnameHash, 0);
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
AtomAnimationDataNode* CreateAnimationDataNode()
{
    AtomAnimationDataNode* node_animData = ACUAllocate<AtomAnimationDataNode>();
    SmallArray_GameType_Reserve(node_animData->InputPorts, 4);
    AtomInputPort* port0 = CreateInputPort(*node_animData, 4, 0);
    AtomInputPort* port1 = CreateInputPort(*node_animData, 4, 0);
    AtomInputPort* port2 = CreateInputPort(*node_animData, 2, 0);
    AtomInputPort* port3 = CreateInputPort(*node_animData, 4, 0);
    AtomOutputPort& defaultOutputPort = node_animData->OutputPorts[0];
    defaultOutputPort.graphNode = node_animData;
    node_animData->AnimationProviderID = 0;
    node_animData->word_66 = 0;
    node_animData->word_68 = 0;
    node_animData->word_6A = 0;
    node_animData->shared_Animation = &static_cast<SharedPtrNew<Animation>&>(FindOrMakeSharedBlockByHandleAndIncrementStrongRefcount(handle_invalidAnimation));
    return node_animData;
}
AtomAnimationRootNode* CreateAnimationRootNode()
{
    AtomAnimationRootNode* node_animRoot = ACUAllocate<AtomAnimationRootNode>();
    SmallArray_GameType_Reserve(node_animRoot->InputPorts, 5);
    AtomInputPort* port0 = CreateInputPort(*node_animRoot, 9, 0);
    AtomInputPort* port1 = CreateInputPort(*node_animRoot, 3, 0);
    AtomInputPort* port2 = CreateInputPort(*node_animRoot, 3, 0);
    AtomInputPort* port3 = CreateInputPort(*node_animRoot, 2, 0);
    AtomInputPort* port4 = CreateInputPort(*node_animRoot, 2, 0);
    AtomOutputPort& defaultOutputPort = *CreateOutputPort(*node_animRoot);
    node_animRoot->CurrentLengthOffset = 20;
    node_animRoot->CurrentRatioOffset = 24;
    node_animRoot->DataOffsetBegin_mb = 0;
    node_animRoot->dword_50 = 8;
    node_animRoot->dword_68 = 4;
    node_animRoot->dword_6C = 28;
    return node_animRoot;
}
AtomGraphStateNode* CreateTheSingularDefaultState()
{
    AtomGraphStateNode* theSingularDefaultState = ACUAllocate<AtomGraphStateNode>();

    AtomAnimationDataNode* node_animData = CreateAnimationDataNode();
    AtomAnimationRootNode* node_animRoot = CreateAnimationRootNode();
    node_animRoot->InputPorts[0].BindingType = 9;
    node_animRoot->InputPorts[0].outputPortIfTypeEq9_graphVarIdxIfEq0 = &node_animData->OutputPorts[0];
    node_animRoot->OutputPorts[0].InstanceDataOffset = node_animRoot->outputInstanceDataOffset_mb = 0;
    node_animData->OutputPorts[0].InstanceDataOffset = node_animData->outputInstanceDataOffset_mb = 42;
    node_animData->outputDataOffsetMinus10 = 42 - 10;
    node_animData->outputDataOffsetMinus6 = 42 - 6;
    node_animData->outputDataOffsetMinus2 = 42 - 2;


    node_animData->reverseGraphNodeIdxInGraph = 1;
    node_animRoot->reverseGraphNodeIdxInGraph = 0;

    SmallArrayAppend<AtomGraphNode*>(theSingularDefaultState->Nodes, node_animData);
    SmallArrayAppend<AtomGraphNode*>(theSingularDefaultState->Nodes, node_animRoot);

    theSingularDefaultState->NumberOfAnimationProviders = 1;
    theSingularDefaultState->InstanceDataSize = 48;

    theSingularDefaultState->word_A2 = 1;
    theSingularDefaultState->word_A4 = 1;
    theSingularDefaultState->word_A6 = 1;
    theSingularDefaultState->word_A8 = 2;
    theSingularDefaultState->dword_98 = 0xFFFFFFFF;

    return theSingularDefaultState;
}
void SetupStateMachineDefaultInitialState(AtomStateMachineNode& stateMachineNode)
{
    AtomInitialState* defaultInitialState = SmallArray_GameType_Append(stateMachineNode.InitialStates);
    AtomConditionExpression* defaultConditionExpression = ACUAllocate<AtomConditionExpression>();
    defaultInitialState->conditionExpression = defaultConditionExpression;
}
void SetupTheWeirdCondition(AtomCondition& cond)
{
    cond.ConditionType = AtomCondition_ConditionType::WEIRD_CONDITION;
    cond.ConditionalOperator = AtomCondition_ConditionalOperator::EQUALS;
    cond.ConjunctionOperator = AtomCondition_ConjunctionOperator::AND;
    cond.word_C = 1;

    cond.ComparisonValue.DataType_0bool1float2int4xy5xyz6quat = AtomDataContainerWrapper_DataType::Weird_Datatype_In_Weird_Conditions;
    std::memset(&cond.ComparisonValue.value, 0, 0x10);
}
void SetupTwoFirstWeirdConditions(AtomConditionExpression& conditionExpr)
{
    AtomCondition* cond1 = SmallArray_GameType_Append(conditionExpr.Conditions);
    AtomCondition* cond2 = SmallArray_GameType_Append(conditionExpr.Conditions);
    SetupTheWeirdCondition(*cond1);
    SetupTheWeirdCondition(*cond2);
}
AtomConditionExpression* CreateConditionExpression_TwoFirstWeirdConditions()
{
    AtomConditionExpression* condExpr = ACUAllocate<AtomConditionExpression>();
    {
        // These are apparently important.
        SetupTwoFirstWeirdConditions(*condExpr);
    }
    return condExpr;
}
AtomConditionExpression* CreateConditionExpression_SingleVariableEqualsTo(uint32 rtcpVarIdx, bool value)
{
    AtomConditionExpression* condExpr = CreateConditionExpression_TwoFirstWeirdConditions();
    {
        AtomCondition* myVarEqualsTo1 = SmallArray_GameType_Append(condExpr->Conditions);
        myVarEqualsTo1->ConditionType = AtomCondition_ConditionType::GRAPH_VARIABLE;
        myVarEqualsTo1->ConditionalOperator = AtomCondition_ConditionalOperator::EQUALS;
        myVarEqualsTo1->ConjunctionOperator = AtomCondition_ConjunctionOperator::AND;
        myVarEqualsTo1->ValueToTestReferenceID = rtcpVarIdx;
        myVarEqualsTo1->ComparisonValue.DataType_0bool1float2int4xy5xyz6quat = AtomDataContainerWrapper_DataType::Bool;
        myVarEqualsTo1->ComparisonValue.value = value;

        SmallArrayAppend(condExpr->referencedGraphVarsIndices, rtcpVarIdx);
    }
    return condExpr;
}
AtomConditionExpression* CreateConditionExpression_PlaybackFinished()
{
    AtomConditionExpression* condExpr = CreateConditionExpression_TwoFirstWeirdConditions();
    {
        AtomCondition* animationIsFinished = SmallArray_GameType_Append(condExpr->Conditions);
        animationIsFinished->ConditionType = AtomCondition_ConditionType::PLAYBACK_PERCENTAGE_mb;
        animationIsFinished->ConditionalOperator = AtomCondition_ConditionalOperator::EQUALS;
        animationIsFinished->ConjunctionOperator = AtomCondition_ConjunctionOperator::AND;
        animationIsFinished->ValueToTestReferenceID = 0;
        animationIsFinished->ComparisonValue.DataType_0bool1float2int4xy5xyz6quat = AtomDataContainerWrapper_DataType::Float;
        (float&)animationIsFinished->ComparisonValue.value = 1.0f;
        animationIsFinished->word_C = 1;
    }
    return condExpr;
}
AtomStateMachineNode* SetupTheNewLayersStateMachine()
{
    AtomStateMachineNode& newLayerStateMachine = *ACUAllocate<AtomStateMachineNode>();

    AtomNullStateNode* theNullState = ACUAllocate<AtomNullStateNode>();
    theNullState->parentNode_mb = &newLayerStateMachine;
    SmallArrayAppend<AtomStateNode*>(newLayerStateMachine.States, theNullState);

    AtomGraphStateNode* theSingularDefaultState = CreateTheSingularDefaultState();
    theSingularDefaultState->parentNode_mb = &newLayerStateMachine;
    SmallArrayAppend<AtomStateNode*>(newLayerStateMachine.States, theSingularDefaultState);

    SetupStateMachineDefaultInitialState(newLayerStateMachine);



    AtomStateTransitionTarget* transitionFromNullStateIfMyVariableIsSet = ACUAllocate<AtomStateTransitionTarget>();
    {
        transitionFromNullStateIfMyVariableIsSet->TargetStateIndex = 1;
        transitionFromNullStateIfMyVariableIsSet->TransitionTime = 0.5f;
        transitionFromNullStateIfMyVariableIsSet->BlendType = 1;
        AtomConditionExpression* condExpr = CreateConditionExpression_SingleVariableEqualsTo(539, true);
        transitionFromNullStateIfMyVariableIsSet->conditionExpression = condExpr;
    }
    AtomStateTransitionTarget* transitionToNullState = ACUAllocate<AtomStateTransitionTarget>();
    {
        transitionToNullState->TargetStateIndex = 0;
        transitionToNullState->TransitionTime = 0.5f;
        transitionToNullState->BlendType = 1;
        AtomConditionExpression* condExpr = CreateConditionExpression_SingleVariableEqualsTo(539, false);
        //AtomConditionExpression* condExpr = CreateConditionExpression_PlaybackFinished();
        transitionToNullState->conditionExpression = condExpr;
    }

    SmallArrayAppend(newLayerStateMachine.transitionTargets_98, transitionFromNullStateIfMyVariableIsSet);
    SmallArrayAppend(newLayerStateMachine.transitionTargets_98, transitionToNullState);

    return &newLayerStateMachine;
}
void SetupTheNewLayer(AtomLayeringInfo& newLayer)
{
    AtomStateMachineNode* newStateMachineNode = SetupTheNewLayersStateMachine();
    newLayer.stateNode38 = newStateMachineNode;

    // If `0 <= dword_50 <= 10`, then all the nearby NPCs are affected.
    // In the "main layering state", all the layers have this value at `20`,
    // and at this value only the player and guards are affected, but not the civilians.
    //newLayer.dword_50 = 20;
    //newLayer.dword_54 = 1;
    //newLayer.dword_58 = 743623600;

    //BoneWeight* newBoneWeight = SmallArray_GameType_Append(newLayer.BoneWeights);
    //newBoneWeight->BoneID = 1806956327;
    //newBoneWeight->WeightF8 = 0xff;
    //newLayer.BlendType = 0;

    SmallArrayAppend(newLayer.arr0, (uint32)0xFFFFFFFF);
    SmallArrayAppend(newLayer.arr0, (uint32)0xFFFFFFFF);
}
void LetsDoSomePlayerGraphMods(AtomGraph& atomGraph)
{
    AddMyNewRTCPVariable(atomGraph);
    auto* mainLayeringState = static_cast<AtomLayeringStateNode*>(atomGraph.RootStateMachine->States[0]);
    AtomLayeringInfo* newLayer = SmallArray_GameType_Append(mainLayeringState->layeringInfos);
    SetupTheNewLayer(*newLayer);
    newLayer->stateNode38->parentNode_mb = mainLayeringState;
    CreateInputPort(*mainLayeringState->someGraphNodesRelatedToExtraLayers[0], 3, 1.0f);
    CreateInputPort(*mainLayeringState->someGraphNodesRelatedToExtraLayers[0], 3, 1.0f);

}

void LetsDoSomePlayerGraphMods()
{
    ACUSharedPtr_Strong<AtomGraph> shared_playerAtomGraph(handle_PlayerAtomGraph);
    if (AtomGraph* atomGraph = shared_playerAtomGraph.GetPtr())
    {
        LetsDoSomePlayerGraphMods(*atomGraph);
    }
}

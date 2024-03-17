#include "pch.h"

#include "AnimationTools/AnimGraphMods.h"
#include "AnimationTools/AtomGraphModsCommon.h"
#include "Common_Plugins/Common_PluginSide.h"
#include "MyAnimationPlayer.h"
namespace fs = std::filesystem;

AtomAnimationDataNode& GraphStateNode_PushAnimationDataNode(AtomGraphStateNode& graphStateNode, uint64 animHandle)
{
    AtomAnimationDataNode* node_animData = ACUAllocate<AtomAnimationDataNode>();

    node_animData->AnimationProviderID = graphStateNode.NumberOfAnimationProviders;
    graphStateNode.NumberOfAnimationProviders += 1;
    node_animData->reverseGraphNodeIdxInGraph = graphStateNode.Nodes.size;
    SmallArrayInsert(graphStateNode.Nodes, static_cast<AtomGraphNode*>(node_animData), 0);
    graphStateNode.InstanceDataSize += 16;

    node_animData->word_66 = node_animData->AnimationProviderID;
    // Not always the same, See AtomGraphStateNode ({16,128,0,240,128,0,128,0,128,0,128,0,128,32}, 0) [[[[[[[[[[[[[[[[[[[[[[[[[[14521AAD0]+40]]+10]+60]+218]+0]+c8]+710]+1c90]+d0]+670]+10]+80]+0]+b8+38]+80]+0]+80]+0]+80]+0]+80]+0]+80]+20]
    // In that graphStateNode, all 4 animation providers have `word_68 == word_66+1`
    node_animData->word_68 = node_animData->word_66;
    node_animData->word_6A = node_animData->word_68;

    SmallArray_GameType_Reserve(node_animData->InputPorts, 4);
    AtomInputPort* port0 = CreateInputPort(*node_animData, 4, 0);
    AtomInputPort* port1 = CreateInputPort(*node_animData, 4, 0);
    AtomInputPort* port2 = CreateInputPort(*node_animData, 2, 0);
    AtomInputPort* port3 = CreateInputPort(*node_animData, 4, 0);
    AtomOutputPort& defaultOutputPort = node_animData->OutputPorts[0];
    defaultOutputPort.graphNode = node_animData;
    node_animData->shared_Animation = &static_cast<SharedPtrNew<Animation>&>(FindOrMakeSharedBlockByHandleAndIncrementStrongRefcount(animHandle));

    const uint32 outputDataOffset = graphStateNode.InstanceDataSize - 16 + 10;

    defaultOutputPort.InstanceDataOffset = outputDataOffset;
    node_animData->outputDataOffsetMinus10 = outputDataOffset - 10;
    node_animData->outputDataOffsetMinus6 = outputDataOffset - 6;
    node_animData->outputDataOffsetMinus2 = outputDataOffset - 2;
    return *node_animData;
}
AtomAnimationRootNode& GraphStateNode_PushAnimationRootNode(AtomGraphStateNode& graphStateNode)
{
    AtomAnimationRootNode* node_animRoot = ACUAllocate<AtomAnimationRootNode>();

    node_animRoot->reverseGraphNodeIdxInGraph = graphStateNode.Nodes.size;
    SmallArrayInsert(graphStateNode.Nodes, static_cast<AtomGraphNode*>(node_animRoot), 0);
    graphStateNode.InstanceDataSize += 32;

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
    defaultOutputPort.InstanceDataOffset = node_animRoot->outputInstanceDataOffset_mb = 0;
    return *node_animRoot;
}
void AnimationRootNode_SetAnimationInput(AtomAnimationRootNode& animRoot, AtomAnimationDataBaseNode& animationInput)
{
    animRoot.InputPorts[0].BindingType = 9;
    animRoot.InputPorts[0].outputPortIfTypeEq9_graphVarIdxIfEq0 = &animationInput.OutputPorts[0];
}
AtomGraphStateNode& CreateGraphState_SimpleAnimation(uint64 animationHandle)
{
    AtomGraphStateNode* newGraphState = ACUAllocate<AtomGraphStateNode>();

    AtomAnimationRootNode* node_animRoot = &GraphStateNode_PushAnimationRootNode(*newGraphState);
    AtomAnimationDataNode* node_animData = &GraphStateNode_PushAnimationDataNode(*newGraphState, animationHandle);
    AnimationRootNode_SetAnimationInput(*node_animRoot, *node_animData);

    newGraphState->numberOfAnimationProviders_alsoMb = newGraphState->NumberOfAnimationProviders;
    newGraphState->word_A4 = 1;
    newGraphState->word_A6 = 1;
    newGraphState->numNodes_mb = 2;
    newGraphState->dword_98 = 0xFFFFFFFF;

    return *newGraphState;
}
AtomGraphStateNode& CreateGraphState_SimpleAnimationImported(const fs::path& jsonAnimFilepath)
{
    AtomGraphStateNode* newGraphState = ACUAllocate<AtomGraphStateNode>();
    ACUSharedPtr_Strong<Animation> loadedAnim = g_NewAnimationsFactory.LoadNewAnimationFromFile(jsonAnimFilepath);
    AtomAnimationRootNode* node_animRoot = &GraphStateNode_PushAnimationRootNode(*newGraphState);
    AtomAnimationDataNode* node_animData = &GraphStateNode_PushAnimationDataNode(*newGraphState, loadedAnim.GetSharedBlock().handle);
    AnimationRootNode_SetAnimationInput(*node_animRoot, *node_animData);

    newGraphState->numberOfAnimationProviders_alsoMb = newGraphState->NumberOfAnimationProviders;
    newGraphState->word_A4 = 1;
    newGraphState->word_A6 = 1;
    newGraphState->numNodes_mb = 2;
    newGraphState->dword_98 = 0xFFFFFFFF;

    return *newGraphState;
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
        // Appears to represent something related to "parentheses" within the `AtomConditionExpression`.
        // Like, how many of the conditions that follow this one need to be "skipped".
        // If so, then it would make sense that the last condition in an expression should have this value at 0.
        animationIsFinished->word_C = 0;
    }
    return condExpr;
}
AtomConditionExpression* CreateConditionExpression_PlaybackPercentage(const float inRange0_1)
{
    AtomConditionExpression* condExpr = CreateConditionExpression_TwoFirstWeirdConditions();
    {
        AtomCondition* animationIsFinished = SmallArray_GameType_Append(condExpr->Conditions);
        animationIsFinished->ConditionType = AtomCondition_ConditionType::PLAYBACK_PERCENTAGE_mb;
        animationIsFinished->ConditionalOperator = AtomCondition_ConditionalOperator::EQUALS;
        animationIsFinished->ConjunctionOperator = AtomCondition_ConjunctionOperator::AND;
        animationIsFinished->ValueToTestReferenceID = 0;
        animationIsFinished->ComparisonValue.DataType_0bool1float2int4xy5xyz6quat = AtomDataContainerWrapper_DataType::Float;
        (float&)animationIsFinished->ComparisonValue.value = inRange0_1;
        // Appears to represent something related to "parentheses" within the `AtomConditionExpression`.
        // Like, how many of the conditions that follow this one need to be "skipped".
        // If so, then it would make sense that the last condition in an expression should have this value at 0.
        animationIsFinished->word_C = 0;
    }
    return condExpr;
}
AtomConditionExpression* CreateConditionExpression_SomeWeirdIntegerSignal()
{
    //Check out transition conditions in nodes at:
    //[[[[[[[[[[[[[[[[[[[14521AAD0]+40]]+10]+60]+218]+0]+c8]+710]+1c90]+d0]+670]+10]+80]+0]+80]+660+38]+80]+0]
    //[[[[[[[[[[[[[[[[[[[14521AAD0]+40]]+10]+60]+218]+0]+c8]+710]+1c90]+d0]+670]+10]+80]+0]+80]+660+38]+80]+20]
    AtomConditionExpression* condExpr = CreateConditionExpression_TwoFirstWeirdConditions();
    {
        AtomCondition* weirdSignal = SmallArray_GameType_Append(condExpr->Conditions);
        weirdSignal->ConditionType = AtomCondition_ConditionType::UNK_4;
        weirdSignal->ConditionalOperator = AtomCondition_ConditionalOperator::EQUALS;
        weirdSignal->ConjunctionOperator = AtomCondition_ConjunctionOperator::AND;
        weirdSignal->MarkUpQueryScope = 2;
        weirdSignal->ValueToTestReferenceID = 0x800042; // 0x800040, 0x800041, 0x800042
        weirdSignal->ComparisonValue.DataType_0bool1float2int4xy5xyz6quat = AtomDataContainerWrapper_DataType::Weird_Datatype_In_Weird_Conditions;
        (float&)weirdSignal->ComparisonValue.value = 0;
        weirdSignal->word_C = 0;
    }
    return condExpr;
}
void StateMachine_PushState(AtomStateMachineNode& stateMachine, AtomStateNode& newState)
{
    newState.parentNode_mb = &stateMachine;
    SmallArrayAppend<AtomStateNode*>(stateMachine.States, &newState);
}

namespace AnimGraphMods::BasicLayer
{
void SetupStateMachineDefaultInitialState(AtomStateMachineNode& stateMachineNode)
{
    AtomInitialState* defaultInitialState = SmallArray_GameType_Append(stateMachineNode.InitialStates);
    defaultInitialState->TargetStateIndex = 0;
    AtomConditionExpression* defaultConditionExpression = ACUAllocate<AtomConditionExpression>();
    defaultInitialState->conditionExpression = defaultConditionExpression;
}
void SetupTheNewLayersStateMachine(AtomStateMachineNode& newLayerStateMachine)
{
    AtomNullStateNode& nullState1 = *ACUAllocate<AtomNullStateNode>();
    AtomGraphStateNode& playbackState1 = CreateGraphState_SimpleAnimationImported(AbsolutePathInThisDLLDirectory("NewAnimations/ACVI_xy_UpperBody_Hood_tr_Hat.anim.json"));
    AtomNullStateNode& nullState2 = *ACUAllocate<AtomNullStateNode>();
    AtomGraphStateNode& playbackState2 = CreateGraphState_SimpleAnimationImported(AbsolutePathInThisDLLDirectory("NewAnimations/ACVI_xy_UpperBody_Hat_tr_Hood.anim.json"));
    //AtomGraphStateNode& playbackState2 = CreateGraphState_SimpleAnimation(handle_anim_Quickdrop);
    StateMachine_PushState(newLayerStateMachine, nullState1);
    StateMachine_PushState(newLayerStateMachine, playbackState1);
    StateMachine_PushState(newLayerStateMachine, nullState2);
    StateMachine_PushState(newLayerStateMachine, playbackState2);

    SetupStateMachineDefaultInitialState(newLayerStateMachine);



    AtomStateTransitionTarget* transitionFromNullStateIfMyVariableIsSet = ACUAllocate<AtomStateTransitionTarget>();
    {
        transitionFromNullStateIfMyVariableIsSet->TargetStateIndex = 1;
        transitionFromNullStateIfMyVariableIsSet->TransitionTime = 0.5f;
        transitionFromNullStateIfMyVariableIsSet->BlendType = 1;
        AtomConditionExpression* condExpr = CreateConditionExpression_SingleVariableEqualsTo(539, true);
        transitionFromNullStateIfMyVariableIsSet->conditionExpression = condExpr;
    }
    AtomStateTransitionTarget* transitionTo2ndNullState = ACUAllocate<AtomStateTransitionTarget>();
    {
        transitionTo2ndNullState->TargetStateIndex = 2;
        transitionTo2ndNullState->TransitionTime = 0.5f;
        transitionTo2ndNullState->BlendType = 1;
        //AtomConditionExpression* condExpr = CreateConditionExpression_SingleVariableEqualsTo(539, false);
        AtomConditionExpression* condExpr = CreateConditionExpression_PlaybackPercentage(0.65f);
        transitionTo2ndNullState->conditionExpression = condExpr;
    }
    AtomStateTransitionTarget* transitionFrom2ndNullStateIfMyVariableIsUNSET = ACUAllocate<AtomStateTransitionTarget>();
    {
        transitionFrom2ndNullStateIfMyVariableIsUNSET->TargetStateIndex = 3;
        transitionFrom2ndNullStateIfMyVariableIsUNSET->TransitionTime = 0.5f;
        transitionFrom2ndNullStateIfMyVariableIsUNSET->BlendType = 1;
        AtomConditionExpression* condExpr = CreateConditionExpression_SingleVariableEqualsTo(539, false);
        transitionFrom2ndNullStateIfMyVariableIsUNSET->conditionExpression = condExpr;
    }
    AtomStateTransitionTarget* transitionTo1stNullState = ACUAllocate<AtomStateTransitionTarget>();
    {
        transitionTo1stNullState->TargetStateIndex = 0;
        transitionTo1stNullState->TransitionTime = 0.5f;
        transitionTo1stNullState->BlendType = 1;
        AtomConditionExpression* condExpr = CreateConditionExpression_PlaybackPercentage(0.65f);
        transitionTo1stNullState->conditionExpression = condExpr;
    }
    AtomStateTransitionTarget* resetPlayback1 = ACUAllocate<AtomStateTransitionTarget>();
    {
        resetPlayback1->TargetStateIndex = 3;
        resetPlayback1->TransitionTime = 0.25f;
        resetPlayback1->BlendType = 1;
        AtomConditionExpression* condExpr = CreateConditionExpression_SingleVariableEqualsTo(539, false);
        resetPlayback1->conditionExpression = condExpr;
    }
    AtomStateTransitionTarget* resetPlayback2 = ACUAllocate<AtomStateTransitionTarget>();
    {
        resetPlayback2->TargetStateIndex = 1;
        resetPlayback2->TransitionTime = 0.25f;
        resetPlayback2->BlendType = 1;
        AtomConditionExpression* condExpr = CreateConditionExpression_SingleVariableEqualsTo(539, true);
        resetPlayback2->conditionExpression = condExpr;
    }

    //SmallArrayAppend(newLayerStateMachine.transitionTargets_98, transitionFromNullStateIfMyVariableIsSet);
    //SmallArrayAppend(newLayerStateMachine.transitionTargets_98, transitionToNullState);
    SmallArrayAppend(nullState1.base8.Transitions, transitionFromNullStateIfMyVariableIsSet);
    SmallArrayAppend(nullState2.base8.Transitions, transitionFrom2ndNullStateIfMyVariableIsUNSET);
    SmallArrayAppend(playbackState1.base8.Transitions, transitionTo2ndNullState);
    SmallArrayAppend(playbackState1.base8.Transitions, resetPlayback1);
    SmallArrayAppend(playbackState2.base8.Transitions, transitionTo1stNullState);
    SmallArrayAppend(playbackState2.base8.Transitions, resetPlayback2);
}
void SetupTheNewLayer(AtomLayeringInfo& newLayer)
{
    SetupTheNewLayersStateMachine(*static_cast<AtomStateMachineNode*>(newLayer.stateNode38));

    // If `0 <= dword_50 <= 10`, then all the nearby NPCs are affected.
    // In the "main layering state", all the layers have this value at `20`,
    // and at this value only the player and guards are affected, but not the civilians.
    //newLayer.dword_50 = 20;
    //newLayer.dword_54 = 1;
    //newLayer.dword_58 = 743623600;

    BoneWeight* boneWeight_upperTorso = SmallArray_GameType_Append(newLayer.BoneWeights);
    //boneWeight_upperTorso->BoneID = 1806956327;
    //boneWeight_upperTorso->BoneID = 1393476043; //"Spine"
    boneWeight_upperTorso->BoneID = 372288500; //"Spine2"
    boneWeight_upperTorso->WeightF8 = 0xFF;
    BoneWeight* boneWeight_leftShoulder = SmallArray_GameType_Append(newLayer.BoneWeights);
    boneWeight_leftShoulder->BoneID = 3097015817; //left shoulder
    boneWeight_leftShoulder->WeightF8 = 0;
    newLayer.BlendType = 0;
}
// In the player's AtomGraph, the `stateNode38` of every `AtomLayeringStateNode` seems to always be
// an instance of `AtomStateMachineNode` and not any other subclass of `AtomStateNode`.
AtomLayeringInfo& LayeringState_CreateNewLayerWithStateMachine(AtomLayeringStateNode& layeringState)
{
    AtomLayeringInfo* newLayer = SmallArray_GameType_Append(layeringState.layeringInfos);
    newLayer->stateNode38 = ACUAllocate<AtomStateMachineNode>();
    newLayer->stateNode38->parentNode_mb = &layeringState;
    SmallArrayAppend(newLayer->arr0, (uint32)0xFFFFFFFF);
    SmallArrayAppend(newLayer->arr0, (uint32)0xFFFFFFFF);
    CreateInputPort(*layeringState.someGraphNodesRelatedToExtraLayers[0], 3, 1.0f);
    CreateInputPort(*layeringState.someGraphNodesRelatedToExtraLayers[0], 3, 1.0f);
    return *newLayer;
}
void ApplyMod(AtomGraph& atomGraph)
{
    AddMyNewRTCPVariable(atomGraph);
    auto* mainLayeringState = static_cast<AtomLayeringStateNode*>(atomGraph.RootStateMachine->States[0]);
    AtomLayeringInfo& newLayer = LayeringState_CreateNewLayerWithStateMachine(*mainLayeringState);
    SetupTheNewLayer(newLayer);
}
}
void AnimGraphMods_BasicLayer_ApplyMod(AtomGraph& atomGraph)
{
    AnimGraphMods::BasicLayer::ApplyMod(atomGraph);
}

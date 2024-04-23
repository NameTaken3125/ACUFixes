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
AtomConditionExpression* CreateConditionExpression_SingleVariableEqualsTo(uint32 rtcpVarIdx, bool value);
AtomConditionExpression* CreateConditionExpression_PlaybackPercentage(const float inRange0_1);
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





class MyCondition
{
public:
    virtual ~MyCondition() {}
};
union ComparisonValue_t
{
    bool bool_;
    int int_;
    float float_;
    __m128 xyzw;
    ComparisonValue_t(int int_) : xyzw{ 0 } { this->int_ = int_; }
    ComparisonValue_t(bool bool_) : xyzw{ 0 } { this->bool_ = bool_; }
    ComparisonValue_t(float float_) : xyzw{ 0 } { this->float_ = float_; }
};
class MyCondition_GraphVariable : public MyCondition
{
public:
    MyCondition_GraphVariable(uint32 rtcpVarIdx, AtomCondition_ConditionalOperator comparisonOp, AtomDataContainerWrapper_DataType varType, ComparisonValue_t value)
        : comparisonOp(comparisonOp)
        , rtcpVarIdx(rtcpVarIdx)
        , varType(varType)
        , value(value)
    {}
    AtomCondition_ConditionalOperator comparisonOp;
    uint32 rtcpVarIdx;
    AtomDataContainerWrapper_DataType varType;
    ComparisonValue_t value;
};
class MyCondition_PlaybackPercentage : public MyCondition
{
public:
    MyCondition_PlaybackPercentage(float inRange0_1) : playbackPercentageInRange0_1(inRange0_1) {}
    float playbackPercentageInRange0_1;
};
using Subconditions_t = std::vector<std::shared_ptr<MyCondition>>;
class MyCondition_Group : public MyCondition
{
public:
    MyCondition_Group(AtomCondition_ConjunctionOperator conjunctionOp, Subconditions_t&& subconditions)
        : conjunctionOp(conjunctionOp)
        , subconditions(std::move(subconditions))
    {}
    AtomCondition_ConjunctionOperator conjunctionOp;
    Subconditions_t subconditions;
};
void SortedSetInsert(SmallArray<uint32>& sortedArrayOfRtcpVarIndices, uint32 rtcpIdxToAdd)
{
    std::optional<uint16> whereToInsert = sortedArrayOfRtcpVarIndices.size;
    for (uint16 i = 0; i < sortedArrayOfRtcpVarIndices.size; i++)
    {
        if (sortedArrayOfRtcpVarIndices[i] > rtcpIdxToAdd)
        {
            whereToInsert = i;
            break;
        }
        else if (sortedArrayOfRtcpVarIndices[i] == rtcpIdxToAdd)
        {
            whereToInsert.reset();
            break;
        }
    }
    if (whereToInsert)
    {
        SmallArrayInsert(sortedArrayOfRtcpVarIndices, rtcpIdxToAdd, *whereToInsert);
    }
}
void AppendConditionGroup(AtomConditionExpression& condExpr, const MyCondition_Group& conditionGroup)
{
    {
        AtomCondition& cond = *SmallArray_GameType_Append(condExpr.Conditions);
        cond.ConditionType = AtomCondition_ConditionType::WEIRD_CONDITION;
        cond.ConditionalOperator = AtomCondition_ConditionalOperator::EQUALS;
        cond.ConjunctionOperator = AtomCondition_ConjunctionOperator::AND;
        cond.word_C = (uint16)conditionGroup.subconditions.size();
    }

    for (const auto& subcondition : conditionGroup.subconditions)
    {
        if (auto* condGraphVar = dynamic_cast<MyCondition_GraphVariable*>(subcondition.get()))
        {
            AtomCondition& cond = *SmallArray_GameType_Append(condExpr.Conditions);
            cond.ConditionType = AtomCondition_ConditionType::GRAPH_VARIABLE;
            cond.word_A = 0xFFFF;
            cond.word_C = 0;
            cond.ValueToTestReferenceID = condGraphVar->rtcpVarIdx;
            cond.ComparisonValue.DataType_0bool1float2int4xy5xyz6quat = condGraphVar->varType;
            (ComparisonValue_t&)cond.ComparisonValue.value = condGraphVar->value;

            cond.ConjunctionOperator = conditionGroup.conjunctionOp;

            SortedSetInsert(condExpr.referencedGraphVarsIndices, condGraphVar->rtcpVarIdx);
        }
        else if (auto* condPlaybackPercentage = dynamic_cast<MyCondition_PlaybackPercentage*>(subcondition.get()))
        {
            AtomCondition* animationReachedPercentage = SmallArray_GameType_Append(condExpr.Conditions);
            animationReachedPercentage->ConditionType = AtomCondition_ConditionType::PLAYBACK_PERCENTAGE_mb;
            animationReachedPercentage->ConditionalOperator = AtomCondition_ConditionalOperator::EQUALS;
            animationReachedPercentage->ConjunctionOperator = AtomCondition_ConjunctionOperator::AND;
            animationReachedPercentage->ValueToTestReferenceID = 0;
            animationReachedPercentage->ComparisonValue.DataType_0bool1float2int4xy5xyz6quat = AtomDataContainerWrapper_DataType::Float;
            (float&)animationReachedPercentage->ComparisonValue.value = condPlaybackPercentage->playbackPercentageInRange0_1;
            animationReachedPercentage->word_C = 0;
        }
        else if (auto* condGroup = dynamic_cast<MyCondition_Group*>(subcondition.get()))
        {
            AppendConditionGroup(condExpr, *condGroup);
        }
    }
}
AtomConditionExpression& BuildConditionExpression(const MyCondition_Group& conditionGroup)
{
    auto& condExpr = *ACUAllocate<AtomConditionExpression>();

    AppendConditionGroup(condExpr, conditionGroup);

    return condExpr;
}
AtomConditionExpression* CreateConditionExpression_SingleVariableEqualsTo(uint32 rtcpVarIdx, bool value)
{
    //AtomConditionExpression* condExpr = CreateConditionExpression_TwoFirstWeirdConditions();
    //{
    //    AtomCondition* myVarEqualsTo1 = SmallArray_GameType_Append(condExpr->Conditions);
    //    myVarEqualsTo1->ConditionType = AtomCondition_ConditionType::GRAPH_VARIABLE;
    //    myVarEqualsTo1->ConditionalOperator = AtomCondition_ConditionalOperator::EQUALS;
    //    myVarEqualsTo1->ConjunctionOperator = AtomCondition_ConjunctionOperator::AND;
    //    myVarEqualsTo1->ValueToTestReferenceID = rtcpVarIdx;
    //    myVarEqualsTo1->ComparisonValue.DataType_0bool1float2int4xy5xyz6quat = AtomDataContainerWrapper_DataType::Bool;
    //    myVarEqualsTo1->ComparisonValue.value = value;

    //    SmallArrayAppend(condExpr->referencedGraphVarsIndices, rtcpVarIdx);
    //}
    //return condExpr;
    return &BuildConditionExpression(MyCondition_Group(
        AtomCondition_ConjunctionOperator::AND,
        Subconditions_t{
            std::make_shared<MyCondition_GraphVariable>(rtcpVarIdx, AtomCondition_ConditionalOperator::EQUALS, AtomDataContainerWrapper_DataType::Bool, ComparisonValue_t(value)),
        }
    ));
}
AtomConditionExpression* CreateConditionExpression_SingleVariableEqualsTo(uint32 rtcpVarIdx, int value)
{
    return &BuildConditionExpression(MyCondition_Group(
        AtomCondition_ConjunctionOperator::AND,
        Subconditions_t{
            std::make_shared<MyCondition_GraphVariable>(rtcpVarIdx, AtomCondition_ConditionalOperator::EQUALS, AtomDataContainerWrapper_DataType::Int, ComparisonValue_t(value)),
        }
    ));
}
AtomConditionExpression* CreateConditionExpression_PlaybackPercentage(const float inRange0_1)
{
    //AtomConditionExpression* condExpr = CreateConditionExpression_TwoFirstWeirdConditions();
    //{
    //    AtomCondition* animationIsFinished = SmallArray_GameType_Append(condExpr->Conditions);
    //    animationIsFinished->ConditionType = AtomCondition_ConditionType::PLAYBACK_PERCENTAGE_mb;
    //    animationIsFinished->ConditionalOperator = AtomCondition_ConditionalOperator::EQUALS;
    //    animationIsFinished->ConjunctionOperator = AtomCondition_ConjunctionOperator::AND;
    //    animationIsFinished->ValueToTestReferenceID = 0;
    //    animationIsFinished->ComparisonValue.DataType_0bool1float2int4xy5xyz6quat = AtomDataContainerWrapper_DataType::Float;
    //    (float&)animationIsFinished->ComparisonValue.value = inRange0_1;
    //    // Appears to represent something related to "parentheses" within the `AtomConditionExpression`.
    //    // Like, how many of the conditions that follow this one need to be "skipped".
    //    // If so, then it would make sense that the last condition in an expression should have this value at 0.
    //    animationIsFinished->word_C = 0;
    //}
    //return condExpr;
    return &BuildConditionExpression(MyCondition_Group(
        AtomCondition_ConjunctionOperator::AND,
        Subconditions_t{
            std::make_shared<MyCondition_PlaybackPercentage>(inRange0_1),
        }
    ));
}
void BuildCETest()
{
    AtomConditionExpression* condExpr = &BuildConditionExpression(MyCondition_Group(
        AtomCondition_ConjunctionOperator::OR,
        Subconditions_t{
            std::make_shared<MyCondition_GraphVariable>(539, AtomCondition_ConditionalOperator::EQUALS, AtomDataContainerWrapper_DataType::Bool, ComparisonValue_t(true)),
            std::make_shared<MyCondition_PlaybackPercentage>(0.75f),
        }
    ));
    int nop = 0;
}



#include "Hack_RemovableHood_Animations.h"
namespace AnimGraphMods::BasicLayer
{
void SetupStateMachineDefaultInitialState(AtomStateMachineNode& stateMachineNode)
{
    AtomInitialState* defaultInitialState = SmallArray_GameType_Append(stateMachineNode.InitialStates);
    defaultInitialState->TargetStateIndex = 0;
    AtomConditionExpression* defaultConditionExpression = ACUAllocate<AtomConditionExpression>();
    defaultInitialState->conditionExpression = defaultConditionExpression;
}

/*
How the HoodControls animations work in the animation graph.

I. Layering my animation on top of normal gameplay.
I take the "normal gameplay layering state" (that's `NewDemo_DEV.AtomGraph->RootStateMachine->States[0]`).
I add another layer to it. This new layer will contain a state machine for the new hood animations.
By adding some BoneWeights to the new layer, I limited the animations to the upper torso excluding the left arm
(because I figured it's already occupied by pistols and bombs).

II. The "hood animations" state machine.
High-level view of what I'm trying to achieve here:
At any point during normal gameplay:
1. Player presses a "Toggle Hood" button.
2. Arno plays either "take hood off" or "put hood on" animation depending on whether the hood is _currently_ on or off.
Mid-level view:
1. Player presses a button.
2. Code figures out whether Arno has his hood on or off.
3. Code changes a single RTCP variable.
4. In response, the animation system plays one of 2 animations ("take hood off"/"put hood on") (and smoothly stops playing them).
What I need to do is to figure out how to make a state machine that allows me to play an animation exactly once
upon setting a given RTCP variable to a specific value (and smoothly stop).
This simple functionality is not very easy to achieve. Because receiving feedback from the animation system can be problematic,
there are nonobvious opportunities enabling you to shoot yourself in the foot and to get "stuck" in some way.

I'll describe how I achieved this in stages of development.
The diagrams to various stages should be included somewhere near the distribution of this text, if not included in the repo itself,
and should make things clearer.
The ellipse boxes are nullstates, the boxes are playback states, the arrows are Transitions between states (with conditions stated nearby),
and the leftmost state is the single unconditional Initial state of the whole state machine.
1. Play an animation once after character loads and remain at the last frame of animation.
For this, I need to add a single "play animation" state, and set it up as the single initial state (empty condition == "always true").
2. Play an animation once after character loads then continue playing in a loop.
For this, I take the singular state and add a Transition that simply restarts the same state.
Transition(targetState="same state", condition="playbackPercentage == 100%").
3. Play an animation once when a given RTCP variable is set to a specific value.
Since I want the animation to _not_be_layered_at_all_until_ the specified conditions are satisfied,
I cannot use just one "play animation" state anymore.
Instead, I add a "null state", which will be my single Initial state.
To this initial null state, I add a Transition(targetState="play 'TakeHoodOff'", condition="HoodControlValue == 1").
Now, after the character loads, he will not play the animation until I set the "HoodControlValue" to 1 (e.g. by ticking an ImGui checkbox),
at which point the playback begins and finishes, but what happens next depends on the Transitions that the two states have set up.
If the playback state has no Transitions, then the animation becomes stuck at the last frame, without reacting to changes to "HoodControlValue".
If the playback state has Transition(targetState="same state", condition="playbackPercentage == 100%), then the animation will be replayed in a loop,
without reacting to changes to "HoodControlValue".
Now for the gotchas:
If the playback state has Transition(targetState="defaultNullState", condition="HoodControlValue == 0"), then the animation gets stuck at the last frame
until you manually set "HoodControlValue" to `0`. That's not what we want, obviously.
If the playback state has Transition(targetState="defaultNullState", condition="playbackPercentage == 100%"), then
the animation system transitions to the default nullState, but because the value of "HoodControlValue" remains `1`, it immediately
transitions again into the playback state. As a result, the animation will play in a loop until you manually set "HoodControlValue" to `0`,
at which point it will _finish_playing_ then stop.
This, again, is not what we want (not for the hood animation, at least).
4. Play an animation _exactly_once_ when a given RTCP variable is set to a specific value, without manually stopping it.
For this, I add a third state, "theOtherNullState" and the playback state receives a Transition(targetState="theOtherNullState", condition="playbackPercentage == 100%").
Now, after the character loads, he will not play the animation until I set the "HoodControlValue" to 1,
at which point the playback begins and finishes without playing in a loop or being stuck in the last frame.
However, since the state machine is in "theOtherNullState", and since that one doesn't have any Transitions,
the animation system no longer reacts to changes to "HoodControlValue".
So we have achieved the functionality of "play an animation cleanly exactly once", but it is a single-use functionality
(until the game reloads and the Player Entity rebuilds).
5. Play "take hood off" animation exactly once when "HoodControlValue" is set to `1`, then play "put hood on" exactly once when "HoodControlValue" is set to `0`,
then again "take hood off" when "HoodControlValue==1`, then "put hood on" when "HoodControlValue==0" and so on.
I add a fourth state, the "play 'put hood on'" state.
Each of the four states contains a single Transition to the "next" state. The 'playback' states transition to Nulls
when the playback finishes, and the null states transition to Playback when the "HoodControlValue" is set to appropriate value.
The good news is that within this state machine, whenever you change the "HoodControlValue",
there is no way to get stuck in either the last frame of an animation or in an infinite animation loop.
The bad news is another gotcha:
Notice how there is no way to start playing "take hood off" animation from "theOtherNullState" (without going through the "put hood on")?
There is no way to play "take hood off" again immediately after having played it.
What if you do want to play it for some reason?
- What if the game's cutscene has put the hood back on without going through your custom animation shenanigans, and you want to take it off again?
- What if you yourself have manually toggled the visibility of the hood's Visual components from, say, the ImGui developer menu?
- What if the "take hood off" animation did play, but the hood ended up _not_ being taken off? As in, the animation signal was, uhh, lost in lag?
(I did say that receiving messages from the animation system can be problematic)
We need to further improve this state machine so that either one of the hood animations can be played exactly once, at any point, from either one of the
null states by changing the "HoodControlValue".
To do this, more complicated Transitions and conditions will be used.
Furthermore, the "take hood off" and "put hood on" will no longer exactly correspond to
"HoodControlValue==1" and "HoodControlValue==0".
6. Play either one of "take hood off" or "put hood on" animations exactly once, regardless of whether it was just played.
Basically:
defaultNullState has  Transition(targetState="play 'take hood off'", condition="HoodControlValue==1"), but
theOtherNullState has Transition(targetState="play 'take hood off'", condition="HoodControlValue==3").
the "play 'takeHoodOff'" state has two Transitions:
Transitions[0] = Transition(targetState="theOtherNullState", condition="playbackPercentage==100% AND HoodControlValue==1)
Transitions[1] = Transition(targetState="defaultNullState",  condition="playbackPercentage==100% AND HoodControlValue==3).
Similarly, the values of `0` and `2` are used to play the "put hood on" animation.
Now, if the animation system is in "theOtherNullState" and I need to play the "take hood off" animation, then I can still do that by setting
the "HoodControlValue" to `3`. This will not result in a playback loop but instead end up in the "defaultNullState".
A complication here is that I don't know which state the state machine is currently in.
But I do know the current value of "HoodControlValue", and from that I know which of the null states the state machine _is_in_or_will_soon_end_up_in_.
Knowing that, and knowing which of the two animations I want to be played, I can set the "HoodControlValue" to a new value.
7. Interrupt "take hood off" with "put hood on" and vice versa.
If I tap "hood toggle button" before the "take hood off" animation finishes,
then I'd expect this animation to immediately be interrupted by a "put hood on" animation. This would be nice,
responsive, civilized. With the current state machine, however, I'll end up finishing the "off" animation and only then playing the "on".
To implement this "interrupt", I add a new Transition in each of the two playback states that immediately (smoothly over 0.25 seconds, actually) transition
to the "opposite" playback state if the "HoodControlValue" is one of the "opposite" values.
(Note that in the current implementation, since I am using specifically the visibility of the Visual components as the indicator of the Hood State,
you actually need to wait until the hood Visuals get switched (about halfway through the animation)
before the second tap on the "hood toggle button" will actually interrupt the animation with the opposite one).
*/
void SetupTheNewLayersStateMachine(AtomStateMachineNode& newLayerStateMachine)
{
    AtomNullStateNode& nullState1 = *ACUAllocate<AtomNullStateNode>();
    AtomGraphStateNode& playbackState1 = CreateGraphState_SimpleAnimationImported(AbsolutePathInThisDLLDirectory("NewAnimations/ACVI_xy_UpperBody_Hood_tr_Hat.anim.json"));
    AtomNullStateNode& nullState2 = *ACUAllocate<AtomNullStateNode>();
    AtomGraphStateNode& playbackState2 = CreateGraphState_SimpleAnimationImported(AbsolutePathInThisDLLDirectory("NewAnimations/ACVI_xy_UpperBody_Hat_tr_Hood.anim.json"));
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
        transitionFromNullStateIfMyVariableIsSet->conditionExpression = CreateConditionExpression_SingleVariableEqualsTo(539, TakeHoodOff_normalpath);
    }
    AtomStateTransitionTarget* transitionTo2ndNullState = ACUAllocate<AtomStateTransitionTarget>();
    {
        transitionTo2ndNullState->TargetStateIndex = 2;
        transitionTo2ndNullState->TransitionTime = 0.5f;
        transitionTo2ndNullState->BlendType = 1;
        transitionTo2ndNullState->conditionExpression = &BuildConditionExpression(MyCondition_Group(
            AtomCondition_ConjunctionOperator::AND,
            Subconditions_t{
                std::make_shared<MyCondition_PlaybackPercentage>(0.65f),
                std::make_shared<MyCondition_GraphVariable>(539, AtomCondition_ConditionalOperator::EQUALS, AtomDataContainerWrapper_DataType::Int, ComparisonValue_t(TakeHoodOff_normalpath))
            }
        ));
    }
    AtomStateTransitionTarget* transitionFrom2ndNullStateIfMyVariableIsUNSET = ACUAllocate<AtomStateTransitionTarget>();
    {
        transitionFrom2ndNullStateIfMyVariableIsUNSET->TargetStateIndex = 3;
        transitionFrom2ndNullStateIfMyVariableIsUNSET->TransitionTime = 0.5f;
        transitionFrom2ndNullStateIfMyVariableIsUNSET->BlendType = 1;
        transitionFrom2ndNullStateIfMyVariableIsUNSET->conditionExpression = CreateConditionExpression_SingleVariableEqualsTo(539, 0);
    }
    AtomStateTransitionTarget* transitionTo1stNullState = ACUAllocate<AtomStateTransitionTarget>();
    {
        transitionTo1stNullState->TargetStateIndex = 0;
        transitionTo1stNullState->TransitionTime = 0.5f;
        transitionTo1stNullState->BlendType = 1;
        transitionTo1stNullState->conditionExpression = &BuildConditionExpression(MyCondition_Group(
            AtomCondition_ConjunctionOperator::AND,
            Subconditions_t{
                std::make_shared<MyCondition_PlaybackPercentage>(0.65f),
                std::make_shared<MyCondition_GraphVariable>(539, AtomCondition_ConditionalOperator::EQUALS, AtomDataContainerWrapper_DataType::Int, ComparisonValue_t(PutHoodOn_normalpath))
            }
        ));
    }
    AtomStateTransitionTarget* interruptByOppositeAnimation1 = ACUAllocate<AtomStateTransitionTarget>();
    {
        interruptByOppositeAnimation1->TargetStateIndex = 3;
        interruptByOppositeAnimation1->TransitionTime = 0.25f;
        interruptByOppositeAnimation1->BlendType = 1;
        interruptByOppositeAnimation1->conditionExpression = CreateConditionExpression_SingleVariableEqualsTo(539, PutHoodOn_normalpath);
    }
    AtomStateTransitionTarget* interruptByOppositeAnimation2 = ACUAllocate<AtomStateTransitionTarget>();
    {
        interruptByOppositeAnimation2->TargetStateIndex = 1;
        interruptByOppositeAnimation2->TransitionTime = 0.25f;
        interruptByOppositeAnimation2->BlendType = 1;
        interruptByOppositeAnimation2->conditionExpression = CreateConditionExpression_SingleVariableEqualsTo(539, TakeHoodOff_normalpath);
    }

    SmallArrayAppend(nullState1.base8.Transitions, transitionFromNullStateIfMyVariableIsSet);
    SmallArrayAppend(nullState2.base8.Transitions, transitionFrom2ndNullStateIfMyVariableIsUNSET);
    SmallArrayAppend(playbackState1.base8.Transitions, transitionTo2ndNullState);
    SmallArrayAppend(playbackState1.base8.Transitions, interruptByOppositeAnimation1);
    SmallArrayAppend(playbackState2.base8.Transitions, transitionTo1stNullState);
    SmallArrayAppend(playbackState2.base8.Transitions, interruptByOppositeAnimation2);

    {
        AtomStateTransitionTarget* tr = ACUAllocate<AtomStateTransitionTarget>();
        tr->TargetStateIndex = 3;
        tr->TransitionTime = 0.5f;
        tr->BlendType = 1;
        tr->conditionExpression = CreateConditionExpression_SingleVariableEqualsTo(539, PutHoodOn_ALTpath);
        SmallArrayAppend(nullState1.base8.Transitions, tr);
    }
    {
        AtomStateTransitionTarget* tr = ACUAllocate<AtomStateTransitionTarget>();
        tr->TargetStateIndex = 1;
        tr->TransitionTime = 0.5f;
        tr->BlendType = 1;
        tr->conditionExpression = CreateConditionExpression_SingleVariableEqualsTo(539, TakeHoodOff_ALTpath);
        SmallArrayAppend(nullState2.base8.Transitions, tr);
    }
    {
        AtomStateTransitionTarget* tr = ACUAllocate<AtomStateTransitionTarget>();
        tr->TargetStateIndex = 0;
        tr->TransitionTime = 0.5f;
        tr->BlendType = 1;
        tr->conditionExpression = &BuildConditionExpression(MyCondition_Group(
            AtomCondition_ConjunctionOperator::AND,
            Subconditions_t{
                std::make_shared<MyCondition_PlaybackPercentage>(0.65f),
                std::make_shared<MyCondition_GraphVariable>(539, AtomCondition_ConditionalOperator::EQUALS, AtomDataContainerWrapper_DataType::Int, ComparisonValue_t(TakeHoodOff_ALTpath))
            }
        ));
        SmallArrayAppend(playbackState1.base8.Transitions, tr);
    }
    {
        AtomStateTransitionTarget* tr = ACUAllocate<AtomStateTransitionTarget>();
        tr->TargetStateIndex = 2;
        tr->TransitionTime = 0.5f;
        tr->BlendType = 1;
        tr->conditionExpression = &BuildConditionExpression(MyCondition_Group(
            AtomCondition_ConjunctionOperator::AND,
            Subconditions_t{
                std::make_shared<MyCondition_PlaybackPercentage>(0.65f),
                std::make_shared<MyCondition_GraphVariable>(539, AtomCondition_ConditionalOperator::EQUALS, AtomDataContainerWrapper_DataType::Int, ComparisonValue_t(PutHoodOn_ALTpath))
            }
        ));
        SmallArrayAppend(playbackState2.base8.Transitions, tr);
    }
    {
        AtomStateTransitionTarget* tr = ACUAllocate<AtomStateTransitionTarget>();
        tr->TargetStateIndex = 3;
        tr->TransitionTime = 0.25f;
        tr->BlendType = 1;
        tr->conditionExpression = CreateConditionExpression_SingleVariableEqualsTo(539, PutHoodOn_ALTpath);
        SmallArrayAppend(playbackState1.base8.Transitions, tr);
    }
    {
        AtomStateTransitionTarget* tr = ACUAllocate<AtomStateTransitionTarget>();
        tr->TargetStateIndex = 1;
        tr->TransitionTime = 0.25f;
        tr->BlendType = 1;
        tr->conditionExpression = CreateConditionExpression_SingleVariableEqualsTo(539, TakeHoodOff_ALTpath);
        SmallArrayAppend(playbackState2.base8.Transitions, tr);
    }
}
void AddBoneWeight(AtomLayeringInfo& layer, uint32 boneID, uint8 weightF8)
{
    BoneWeight* boneWeight = SmallArray_GameType_Append(layer.BoneWeights);
    boneWeight->BoneID = boneID;
    boneWeight->WeightF8 = weightF8;
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

    AddBoneWeight(newLayer, 372288500, 0xFF); // "Spine2" == upper torso
    AddBoneWeight(newLayer, 3097015817, 0); //left shoulder
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
    AddMyNewerRTCPVariable(atomGraph, g_newGraphVar);
    auto* mainLayeringState = static_cast<AtomLayeringStateNode*>(atomGraph.RootStateMachine->States[0]);
    {
        AtomLayeringInfo& newLayer = LayeringState_CreateNewLayerWithStateMachine(*mainLayeringState);
        SetupTheNewLayer(newLayer);
    }

    auto* cinematicState = static_cast<AtomStateMachineNode*>(atomGraph.RootStateMachine->States[1]);
    auto* cinematicLayeringState = static_cast<AtomLayeringStateNode*>(cinematicState->States[0]);
    {
        AtomLayeringInfo& newLayer = LayeringState_CreateNewLayerWithStateMachine(*cinematicLayeringState);
        SetupTheNewLayer(newLayer);
    }
}
}
void AnimGraphMods_BasicLayer_ApplyMod(AtomGraph& atomGraph)
{
    AnimGraphMods::BasicLayer::ApplyMod(atomGraph);
}

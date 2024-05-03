#pragma once

#include "ACU/basic_types.h"
#include "ACU/AtomAnimComponent.h"
#include "ACU/AtomGraph.h"
#include "ACU/HumanStatesHolder.h"
#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct AnimationGraphEvaluationPatches : AutoAssemblerCodeHolder_Base
{
    AnimationGraphEvaluationPatches();
};

#include "Common_Plugins/AnimationGraphMods/ReactToAnimationSignals.h"

class DatabaseOfCustomReactionsToAnimationSignals
{
public:
public:
    void RegisterCustomReaction(CustomReactionToAnimationSignals& newCustomReceiver);
    void UnregisterCustomReaction(CustomReactionToAnimationSignals& reactionToRemove);
    void RegisterSignal(SignalID_t signalInt);
    void WhenSignalChangeDispatched(HumanStatesHolder* receivingEntityHumanStates, SignalID_t signalInt, bool isSignalOn);
public:
    void Hook_WhenInitializingArrayOfIntegerSignalReceivers(ManagerOfAnimationSignalsReceivers& integerSignalReceiversManager);
private:
    std::vector<CustomReactionToAnimationSignals*> m_CustomReactions;
public:
    std::set<SignalID_t> m_AppendedNewSignalReceiversInHumanStates;
};
extern DatabaseOfCustomReactionsToAnimationSignals g_DatabaseOfCustomReactionsToAnimationSignals;

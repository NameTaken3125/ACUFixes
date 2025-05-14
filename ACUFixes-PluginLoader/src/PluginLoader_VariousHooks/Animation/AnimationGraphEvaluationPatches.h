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
    void RegisterCustomReaction(AnimationTools::Signals::CustomReactionToAnimationSignals& newCustomReceiver);
    void UnregisterCustomReaction(AnimationTools::Signals::CustomReactionToAnimationSignals& reactionToRemove);
    void RegisterSignal(AnimationTools::Signals::SignalID_t signalInt);
    void WhenSignalChangeDispatched(HumanStatesHolder* receivingEntityHumanStates, AnimationTools::Signals::SignalID_t signalInt, bool isSignalOn);
public:
    void Hook_WhenInitializingArrayOfIntegerSignalReceivers(ManagerOfAnimationSignalsReceivers& integerSignalReceiversManager);
private:
    std::vector<AnimationTools::Signals::CustomReactionToAnimationSignals*> m_CustomReactions;
public:
    std::set<AnimationTools::Signals::SignalID_t> m_AppendedNewSignalReceiversInHumanStates;
public:
    static DatabaseOfCustomReactionsToAnimationSignals& GetSingleton() { static DatabaseOfCustomReactionsToAnimationSignals singleton; return singleton; }
};

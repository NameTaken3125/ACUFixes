#include "pch.h"

#include "Common_Plugins_impl/AnimationModdingInterface.h"
#include "Common_Plugins/AnimationGraphMods/ReactToAnimationSignals.h"
#include "PluginLoader_VariousHooks/Animation/AnimationGraphEvaluationPatches.h"

using AnimationTools::Signals::SignalID_t;
using AnimationTools::Signals::CustomReactionToAnimationSignals;

void RegisterCustomReaction_impl(CustomReactionToAnimationSignals& newCustomReaction);
void UnregisterCustomReaction_impl(CustomReactionToAnimationSignals& reactionToRemove);
void RegisterSignal_impl(SignalID_t signalInt);
void AddNewRTCPVariableIfNotPresent_impl(AtomGraph& atomGraph, const RTCPVariableDescriptor& newVarDescriptor);
AnimationModdingInterface::AnimationModdingInterface()
    : fnp_RegisterSignal(::RegisterSignal_impl)
    , fnp_RegisterCustomReaction(::RegisterCustomReaction_impl)
    , fnp_UnregisterCustomReaction(::UnregisterCustomReaction_impl)
    , fnp_AddNewRTCPVariableIfNotPresent(::AddNewRTCPVariableIfNotPresent_impl)
{
    // To ensure the initialization order of the singletons.
    DatabaseOfCustomReactionsToAnimationSignals::GetSingleton();
}

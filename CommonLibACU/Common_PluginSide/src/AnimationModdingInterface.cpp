#include "pch.h"

#include "Common_Plugins_impl/AnimationModdingInterface.h"
#include "Common_Plugins/AnimationGraphMods/RTCPVariableDescriptor.h"

AnimationModdingInterface* g_AnimationModdingInterface = nullptr;


namespace AnimationTools::Signals
{
void RegisterCustomReaction(CustomReactionToAnimationSignals& newCustomReaction)
{
    g_AnimationModdingInterface->fnp_RegisterCustomReaction(newCustomReaction);
}
void UnregisterCustomReaction(CustomReactionToAnimationSignals& reactionToRemove)
{
    g_AnimationModdingInterface->fnp_UnregisterCustomReaction(reactionToRemove);
}
void RegisterSignal(SignalID_t signalInt)
{
    g_AnimationModdingInterface->fnp_RegisterSignal(signalInt);
}
} // namespace AnimationTools::Signals
void AddNewRTCPVariableIfNotPresent(AtomGraph& atomGraph, const RTCPVariableDescriptor& newVarDescriptor)
{
    g_AnimationModdingInterface->fnp_AddNewRTCPVariableIfNotPresent(atomGraph, newVarDescriptor);
}

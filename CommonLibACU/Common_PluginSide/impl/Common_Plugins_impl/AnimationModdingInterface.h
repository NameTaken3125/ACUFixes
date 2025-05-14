#pragma once


#include "Common_Plugins/AnimationGraphMods/ReactToAnimationSignals.h"

class AtomGraph;
class RTCPVariableDescriptor;

class  AnimationModdingInterface
{
public:
    void (*fnp_RegisterSignal)(AnimationTools::Signals::SignalID_t signalInt);
    void (*fnp_RegisterCustomReaction)(AnimationTools::Signals::CustomReactionToAnimationSignals& newCustomReaction);
    void (*fnp_UnregisterCustomReaction)(AnimationTools::Signals::CustomReactionToAnimationSignals& reactionToRemove);
    void (*fnp_AddNewRTCPVariableIfNotPresent)(AtomGraph& atomGraph, const RTCPVariableDescriptor& newVarDescriptor);

    AnimationModdingInterface(); // Defined in PluginLoader project.
};

extern AnimationModdingInterface* g_AnimationModdingInterface; // Pointer is stored in the plugin; points to the instance owned by the PluginLoader.

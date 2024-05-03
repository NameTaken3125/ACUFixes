#include "pch.h"

#include "Common_Plugins/AnimationGraphMods/RTCPVariableDescriptor.h"
#include "AnimationTools/Hack_RemovableHood_Animations.h"

#include "ACU/AtomGraph.h"
#include "Experimental_StrongPtr.h"

#include "AnimationTools/UsefulHandles.h"

void AnimGraphMods_BasicLayer_ApplyMod(AtomGraph& graph);

void ApplyAnimationGraphMods()
{
    ACUSharedPtr_Strong<AtomGraph> shared_playerAtomGraph(handle_PlayerAtomGraph);
    if (AtomGraph* atomGraph = shared_playerAtomGraph.GetPtr())
    {
        AnimGraphMods_BasicLayer_ApplyMod(*atomGraph);
    }
}

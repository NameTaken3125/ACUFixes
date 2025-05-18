#include "pch.h"

#include "Common_Plugins/AnimationGraphMods/RTCPVariableDescriptor.h"
#include "AnimationTools/Hack_RemovableHood_Animations.h"

#include "ACU/AtomGraph.h"
#include "ACU/ManagedPtrs/ManagedPtrs.h"

#include "AnimationTools/UsefulHandles.h"

void AnimGraphMods_RemovableHoodAnimated_ApplyMod(AtomGraph& graph);

void ApplyAnimationGraphMods()
{
    ACU::StrongRef<AtomGraph> shared_playerAtomGraph(handle_PlayerAtomGraph);
    if (AtomGraph* atomGraph = shared_playerAtomGraph.GetPtr())
    {
        AnimGraphMods_RemovableHoodAnimated_ApplyMod(*atomGraph);
    }
}

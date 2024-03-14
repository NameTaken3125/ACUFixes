#include "pch.h"

#include "AnimationTools/AnimGraphMods.h"
MyNewRTCPVariable g_newGraphVar;

#include "ACU/AtomGraph.h"
#include "Experimental_StrongPtr.h"

#include "AnimationTools/UsefulHandles.h"

void AnimGraphMods_BasicLayer_ApplyMod(AtomGraph& graph);

void LetsDoSomePlayerGraphMods()
{
    ACUSharedPtr_Strong<AtomGraph> shared_playerAtomGraph(handle_PlayerAtomGraph);
    if (AtomGraph* atomGraph = shared_playerAtomGraph.GetPtr())
    {
        AnimGraphMods_BasicLayer_ApplyMod(*atomGraph);
    }
}

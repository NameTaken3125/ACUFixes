#pragma once

#include "ACU/AtomAnimComponent.h"
#include "ACU/AtomGraph.h"

template<typename VarType>
void SetGraphVariable(GraphEvaluation& graphEvaluation, uint32 varnameHash, VarType newValue)
{
    // See example usage at 0x1419C8204, where the graph variable
    // `animationref CinematicAnimation; // 0x7ad472e5/2060743397`
    // is being set.
    uint32* pGraphVarIndex = graphEvaluation.rtcp->atomGraphVarsHashmap.Get(varnameHash);
    if (!pGraphVarIndex)
    {
        return;
    }
    uint32 graphVarIndex = *pGraphVarIndex;
    uint32 graphVarOffsetInVarsBuffer = graphEvaluation.rtcp->graphVarsOffsets[graphVarIndex];
    VarType* varInVarsBuffer = reinterpret_cast<VarType*>(&graphEvaluation.stru400.graphVarsBuffer[graphVarOffsetInVarsBuffer]);
    *varInVarsBuffer = newValue;
    // Set the _bit_ with this index to signify that the corresponding variable was updated.
    graphEvaluation.stru400.bitmaskOfUpdatedGraphVars[graphVarIndex >> 3] |= 1 << (graphVarIndex & 7);
    if (graphEvaluation.stru400.dword_24C > graphVarIndex)
        graphEvaluation.stru400.dword_24C = graphVarIndex;
    if (graphEvaluation.stru400.dword_250 < graphVarIndex)
        graphEvaluation.stru400.dword_250 = graphVarIndex;
}
template<typename VarType>
VarType* GetGraphVariable(GraphEvaluation& graphEvaluation, uint32 varnameHash)
{
    uint32* pGraphVarIndex = graphEvaluation.rtcp->atomGraphVarsHashmap.Get(varnameHash);
    if (!pGraphVarIndex)
    {
        return nullptr;
    }
    uint32 graphVarIndex = *pGraphVarIndex;
    uint32 graphVarOffsetInVarsBuffer = graphEvaluation.rtcp->graphVarsOffsets[graphVarIndex];
    VarType* varInVarsBuffer = reinterpret_cast<VarType*>(&graphEvaluation.stru400.graphVarsBuffer[graphVarOffsetInVarsBuffer]);
    return varInVarsBuffer;
}

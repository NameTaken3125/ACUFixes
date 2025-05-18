#pragma once

#include "ACU_DefineNativeFunction.h"

#include "ACU/ACUHashmap.h"
#include "ACU/AtomGraph.h"

#include "ACU/AtomStateNode.h"
#include "ACU/AtomNullStateNode.h"
#include "ACU/AtomGraphStateNode.h"
#include "ACU/AtomStateMachineNode.h"
#include "ACU/AtomLayeringStateNode.h"
#include "ACU/AtomStateTransitionTarget.h"

#include "ACU/AtomGraphNode.h"
#include "ACU/AtomAnimationDataNode.h"
#include "ACU/AtomAnimationRootNode.h"

#include "ACU/AtomCondition.h"
#include "ACU/AtomConditionExpression.h"

#include "ACU_SharedPtrs.h"
#include "Experimental_StrongPtr.h"

#include "Common_Plugins/AnimationGraphMods/RTCPVariableDescriptor.h"

#include "AnimationTools/UsefulHandles.h"


void AddNewRTCPVariableIfNotPresent(AtomGraph& atomGraph, const RTCPVariableDescriptor& newVarDescriptor);
AtomOutputPort* CreateOutputPort(AtomGraphNode& graphNode)
{
    AtomOutputPort* defaultOutputPort = ACU::Memory::SmallArray_GameType_Append(graphNode.OutputPorts);
    defaultOutputPort->graphNode = &graphNode;
    return defaultOutputPort;
}
union InputPortPossibleValue_t
{
    AtomOutputPort* outport;
    int integer;
    float flt;
    InputPortPossibleValue_t(int integer) : integer(integer) {}
    InputPortPossibleValue_t(float flt) : flt(flt) {}
};
AtomInputPort* CreateInputPort(AtomGraphNode& graphNode, uint32 bindingType, InputPortPossibleValue_t value)
{
    AtomInputPort* newInport = ACU::Memory::SmallArray_GameType_Append(graphNode.InputPorts);
    newInport->BindingType = bindingType;
    (InputPortPossibleValue_t&)newInport->outputPortIfTypeEq9_graphVarIdxIfEq0 = value;
    return newInport;
}

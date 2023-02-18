#pragma once

#include "ACU/ManagedObject.h"
#include <ACU/SmallArray.h>

class BooleanRuleCondition;
class CameraData;
class TransitionInfoHolder;

class CameraSelectorNode : public ManagedObject
{
public:
    char pad_0010[8]; //0x0010
    BooleanRuleCondition* ruleCondition; //0x0018
    CameraData* cameraData; //0x0020
    TransitionInfoHolder** transitionInfoHolders; //0x0028
    char pad_0030[32]; //0x0030
}; //Size: 0x0050
assert_sizeof(CameraSelectorNode, 0x50);

class BlenderParam;
class CameraSelectorBlenderNode : public CameraSelectorNode
{
public:
    SmallArray<BlenderParam*> blenderParams; //0x0050
    char pad_005C[4]; //0x005C
}; //Size: 0x0060
assert_sizeof(CameraSelectorBlenderNode, 0x60);

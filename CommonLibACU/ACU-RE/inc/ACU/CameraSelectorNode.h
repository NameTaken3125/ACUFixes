#pragma once

#include "ACU/ManagedObject.h"
#include <ACU/SmallArray.h>
#include <ACU/SharedPtr.h>

class BooleanRuleCondition;
class CameraData;
class TransitionInfoHolder;

class CameraSelectorNode : public ManagedObject
{
public:
    char pad_0010[8]; //0x0010
    BooleanRuleCondition* ruleCondition; //0x0018
    CameraData* cameraData; //0x0020
    SmallArray<TransitionInfoHolder*> transitionInfoHolders; //0x0028
    SmallArray<SharedPtrNew<CameraSelectorNode>*> sharedSelectorNodes; //0x0034
    char pad_0040[16]; //0x0040
}; //Size: 0x0050
assert_sizeof(CameraSelectorNode, 0x50);

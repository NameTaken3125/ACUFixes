#pragma once

#include "Common_Plugins/AnimationGraphMods/RTCPVariableDescriptor.h"

enum ValueOfHoodControl : int
{
    TakeHoodOff_normalpath = 1,
    TakeHoodOff_ALTpath = 3,
    PutHoodOn_normalpath = 0,
    PutHoodOn_ALTpath = 2,
};

extern RTCPVariableDescriptor g_rtcpDesc_HoodControlValue;

bool IsHoodAnimationsLoaded();

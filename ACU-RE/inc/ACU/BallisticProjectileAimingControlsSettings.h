#pragma once

#include "ManagedObject.h"

class BallisticProjectileAimingControlsSettings : public ManagedObject
{
public:
    // @members
    float sensitivityHorizontal; //0x0010
    float sensitivityVertical; //0x0014
    float flt_18; //0x0018
    char pad_001C[4]; //0x001C

    // @helper_functions

    // Not sure if this really never fails.
    static BallisticProjectileAimingControlsSettings& GetSingleton();
}; //Size: 0x0020
assert_sizeof(BallisticProjectileAimingControlsSettings, 0x20);

#pragma once

#include "ACU/AvatarGear.h"
#include "ACU/AvatarGearModifierStats.h"

class AvatarGearManager : public ManagedObject
{
public:
    // @members
    SmallArray<AvatarGear*> gears_10_shopGears_mb; //0x0010
    SmallArray<AvatarGear*> gears_1C_hasLanternAtEnd; //0x001C
    char pad_28[0x38-0x28]; //0x0028
    AvatarGear gear_38; //0x0038
    AvatarGear gear_D0; //0x00D0
    AvatarGear gear_168; //0x0168
    AvatarGear gear_200; //0x0200
    AvatarGear gear_298; //0x0298
    AvatarGear gear_330; //0x0330
    char pad_03C8[8]; //0x03C8
    AvatarGearModifierStats gearModifierStats; //0x03D0
    char pad_0500[32]; //0x0500

    // @helper_functions
    static AvatarGearManager* GetSingleton();
}; //Size: 0x0520
assert_sizeof(AvatarGearManager, 0x0520);

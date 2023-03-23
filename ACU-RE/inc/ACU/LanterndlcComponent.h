#pragma once

#include "Component.h"
#include "SoundEvent.h"
#include "SharedPtr.h"

class FXInstance;

class LanterndlcComponent : public Component
{
public:
    char pad_0020[184]; //0x0020
    SoundEvent soundEvent_d8; //0x00D8
    char pad_00F8[520]; //0x00F8
    uint8 isInModeAbleToCharge_300; //0x0300
    char pad_0301[151]; //0x0301
    uint64 onExplosionSoundEventPrototype_mb; //0x0398
    SoundEvent soundEvent_3A0; //0x03A0
    char pad_03C0[144]; //0x03C0
    SharedPtrNew<FXInstance>* sharedPtr_fxInstance_nonNullWhenCharging; //0x0450
    char pad_0458[16]; //0x0458
    SharedPtrNew<Entity>* playerSharedPtr; //0x0468
    uint8 isAlight_mb; //0x0470
    char pad_0471[3]; //0x0471
    float oilLeftSeconds; //0x0474
    char pad_0478[144]; //0x0478
    uint8 isOnGround_mb_508; //0x0508
    char pad_0509[3]; //0x0509
    uint8 isCharging_50c; //0x050C
    char pad_050D[3]; //0x050D
}; //Size: 0x0510
assert_sizeof(LanterndlcComponent, 0x510);

#pragma once

#include "Component.h"
#include "SoundEvent.h"
#include "SharedPtr.h"

class FXInstance;
class FX;
class InventoryItemSettings;

class LanterndlcComponent : public Component
{
public:
    char pad_0020[16]; //0x0020
    SharedPtrNew<FX>* fx_30; //0x0030
    char pad_0038[96]; //0x0038
    SharedPtrNew<Entity>* entityAtLanternPositionWithFXComponent_98; //0x0098
    SharedPtrNew<Entity>* entityAtLanternPositionWithFXComponent_A0; //0x00A0
    char pad_00A8[40]; //0x00A8
    SoundInstance sound_d0; //0x00D0
    SoundInstance sound_100; //0x0100
    SoundInstance soundOnUnsheathe; //0x0130
    SoundInstance soundOnSheathe; //0x0160
    SoundInstance sound_190; //0x0190
    SoundInstance sound_1C0; //0x01C0
    SoundInstance sound_1F0; //0x01F0
    SoundInstance sound_220; //0x0220
    SoundInstance sound_250; //0x0250
    SoundInstance sound_280; //0x0280
    SoundInstance sound_2B0; //0x02B0
    uint8 byte_2E0; //0x02E0
    char pad_02E1[7]; //0x02E1
    SharedPtrNew<InventoryItemSettings>* invItemSettings; //0x02E8
    float flt_2f0; //0x02F0
    char pad_02F4[12]; //0x02F4
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

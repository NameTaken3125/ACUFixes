#pragma once

#include "SoundEvent.h"

class ACUGlobalSoundSet
{
public:
    char pad_0000[1448]; //0x0000
    SoundEventWrapper onDisguiseBroken; //0x05A8
    char pad_05D0[56]; //0x05D0
    SoundEventWrapper onFailedDisguiseOrMedicineHealthFull; //0x0608
    char pad_0630[3808]; //0x0630
}; //Size: 0x1510
assert_sizeof(ACUGlobalSoundSet, 0x1510);

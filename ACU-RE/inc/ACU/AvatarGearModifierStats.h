#pragma once

#include "ManagedObject.h"

class AvatarGearModifierStats : public Object
{
public:
    char pad_0008[8]; //0x0008
    float eagleVisionDuration; //0x0010
    float flt_14; //0x0014
    char pad_0018[8]; //0x0018
    float flt_20; //0x0020
    float meleeArmor; //0x0024
    float rangedArmor; //0x0028
    float timeToBlend; //0x002C
    float blendFadeTime; //0x0030
    float restrictedAreaDetectionTime; //0x0034
    float disguiseRadius; //0x0038
    float disguiseAddDuration; //0x003C
    float highProfileNoise; //0x0040
    float lowProfileNoise; //0x0044
    float takedownNoise; //0x0048
    float additionalDamageMelee; //0x004C
    float additionalDamageRanged; //0x0050
    char pad_0054[12]; //0x0054
    float healing; //0x0060
    float fallingDamage; //0x0064
    float fallingNoise; //0x0068
    float runningNoise; //0x006C
    float additionalMoneyLooted; //0x0070
    char pad_0074[12]; //0x0074
    uint32 additionalMedicine; //0x0080
    uint32 additionalSmokeBombs; //0x0084
    uint32 additionalStunGrenades; //0x0088
    char pad_008C[28]; //0x008C
    float eagleVisionRange; //0x00A8
    char pad_00AC[8]; //0x00AC
    uint32 additionalPhantomBlades; //0x00B4
    uint32 additionalBerserkBlades; //0x00B8
    uint32 additionalLockpicks; //0x00BC
    char pad_00C0[96]; //0x00C0
    uint8 stonesThrownByRaidersHaveNoEffect; //0x0120
    uint8 whenHitByBerserkBladeRaiderAttacksLeader; //0x0121
    char pad_0122[14]; //0x0122
}; //Size: 0x0130
assert_sizeof(AvatarGearModifierStats, 0x130);

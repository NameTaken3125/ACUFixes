#pragma once

#include "ACU/ManagedObject.h"
#include "ACU/SharedPtr.h"

class InventoryItemSettings;

class AvatarGear : public Object
{
public:
    char pad_0008[8]; //0x0008
    SharedPtrNew<InventoryItemSettings>* inventoryItemSettings; //0x0010
    SharedPtrNew<InventoryItemSettings>* invItemSett_18; //0x0018
    char pad_0020[120]; //0x0020
}; //Size: 0x0098
assert_offsetof(AvatarGear, inventoryItemSettings, 0x10);
assert_sizeof(AvatarGear, 0x98);
#include "ACU/SmallArray.h"
class AvatarGearManager
{
public:
    char pad_0000[16]; //0x0000
    SmallArray<AvatarGear*> gears_10; //0x0010
    SmallArray<AvatarGear*> gears_1C_hasLanternAtEnd; //0x001C
    void* arr_28; //0x0028
    char pad_0030[964]; //0x0030
    float meleeArmor; //0x03F4
    float rangedArmor; //0x03F8
    float timeToBlend; //0x03FC
    float blendFadeTime; //0x0400
    float restrictedAreaDetectionTime; //0x0404
    float disguiseRadius; //0x0408
    float disguiseAddDuration; //0x040C
    float highProfileNoise; //0x0410
    float lowProfileNoise; //0x0414
    float takedownNoise; //0x0418
    char pad_041C[20]; //0x041C
    float healing; //0x0430
    float fallingDamage; //0x0434
    float fallingNoise; //0x0438
    float runningNoise; //0x043C
    char pad_0440[16]; //0x0440
    uint32 additionalMedicine; //0x0450
    uint32 additionalSmokeBombs; //0x0454
    uint32 additionalStunGrenades; //0x0458
    char pad_045C[48]; //0x045C
    uint32 additionalLockpicks; //0x048C
    char pad_0490[96]; //0x0490
    uint8 stonesThrownByRaidersHaveNoEffect; //0x04F0
    uint8 whenHitByBerserkBladeRaiderAttacksLeader; //0x04F1
    char pad_04F2[46]; //0x04F2
}; //Size: 0x0520
assert_sizeof(AvatarGearManager, 0x0520);

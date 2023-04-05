#pragma once

#include "basic_types.h"
#include "vmath/vmath.h"
#include "Enum_EquipmentType.h"

class BallisticProjectileAimingProcess_190
{
public:
    char pad_0000[28]; //0x0000
    float nearAndMidModeDistance; //0x001C
    float angleUpDown; //0x0020
    char pad_0024[4]; //0x0024
    Vector4f vecForward_sorta_1b8; //0x0028
    float flt_38; //0x0038
    char pad_003C[44]; //0x003C
}; //Size: 0x0068

class BallisticProjectileAimingProcess_118;
class BallisticProjectileSettings;
class BallisticProjectileAimingProcess
{
public:
    char pad_0000[128]; //0x0000
    Vector4f predictionBeamOrigin; //0x0080
    char pad_0090[32]; //0x0090
    Vector4f vecForwardFlat_b0; //0x00B0
    char pad_00C0[72]; //0x00C0
    float flt_108; //0x0108
    char pad_010C[12]; //0x010C
    BallisticProjectileAimingProcess_118* stru_118; //0x0118
    char pad_0120[104]; //0x0120
    BallisticProjectileSettings** projectileSettings_complicatedPtr; //0x0188
    BallisticProjectileAimingProcess_190 currentAimingState; //0x0190
    char pad_01F8[8]; //0x01F8
}; //Size: 0x0200
assert_sizeof(BallisticProjectileAimingProcess, 0x200);

class SharedPtr_mb;

class UsedDuringDisguise
{
public:
    char pad_0000[576]; //0x0000
    SharedPtr_mb* disguiseTargetEntity; //0x0240
    char pad_0248[48]; //0x0248
    uint8 isInDisguise_mb; //0x0278
    char pad_0279[15]; //0x0279
};
assert_offsetof(UsedDuringDisguise, disguiseTargetEntity, 0x240);
class HumanStatesHolder_D0
{
public:
    UsedDuringDisguise* usedDuringDisguise; //0x0000
    char pad_0008[256]; //0x0008
}; //Size: 0x0108

// vtbl: 142FFD128
class HumanStatesHolder
{
public:
    // @members
    char pad_0000[208]; //0x0000
    HumanStatesHolder_D0* humanStatesHolder_D0; //0x00D0
    char pad_00D8[3164]; //0x00D8
    EquipmentType ballisticAimingCurrentEquipmentType; //0x0D34
    char pad_0D38[56]; //0x0D38
    BallisticProjectileAimingProcess aimingSmokeBomb; //0x0D70
    BallisticProjectileAimingProcess aimingMoneyPouch; //0x0F70
    BallisticProjectileAimingProcess aimingCherryBomb; //0x1170
    BallisticProjectileAimingProcess aimingPoison; //0x1370
    BallisticProjectileAimingProcess aimingDefault; //0x1570
    BallisticProjectileAimingProcess aiming_equip19_1770; //0x1770
    BallisticProjectileAimingProcess aimingGuillotineGun; //0x1970
    char pad_1B70[816]; //0x1B70

    // @helper_functions
    static HumanStatesHolder* GetForPlayer();
}; //Size: 0x1EA0
assert_offsetof(HumanStatesHolder, ballisticAimingCurrentEquipmentType, 0xD34);
assert_offsetof(HumanStatesHolder, aimingGuillotineGun, 0x1970);

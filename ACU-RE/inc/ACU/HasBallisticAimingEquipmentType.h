#pragma once

#include "basic_types.h"
#include "vmath/vmath.h"

class BallisticProjectileAimingProcess_190
{
public:
    char pad_0000[28]; //0x0000
    float nearAndMidModeDistance; //0x001C
    float angle_mb; //0x0020
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
    BallisticProjectileAimingProcess_190 stru_190; //0x0190
    char pad_01F8[8]; //0x01F8
}; //Size: 0x0200
assert_sizeof(BallisticProjectileAimingProcess, 0x200);

enum EquipmentType : uint32
{
    unk_1 = 0x1,
    Pistol = 0x2,
    SmokeBomb = 0x5,
    MoneyPouch = 0xD,
    PoisonBomb = 0x13,
    StunBomb = 0x14,
    PhantomBlade = 0x16,
    BerserkBlade = 0x17,
    CherryBomb = 0x18,
    Rifle = 0x1C,
    Blunderbuss = 0x1D,
    GuillotineGun = 0x1E,
    Slot9 = 0x20,
    MAX_mb = 37,
};
enum EquipmentTypeBitMask : uint64
{
    LeftHandWeapons = 0x70C00006,
};

// vtbl: 142FFD128
class HasBallisticAimingEquipmentType
{
public:
    char pad_0000[3380]; //0x0000
    EquipmentType ballisticAimingCurrentEquipmentType; //0x0D34
    char pad_0D38[568]; //0x0D38
    BallisticProjectileAimingProcess aimingMoney; //0x0F70
    BallisticProjectileAimingProcess aiming_equipType0x12_1170; //0x1170
    BallisticProjectileAimingProcess aimingPoison; //0x1370
    BallisticProjectileAimingProcess aimingDefault; //0x1570
    BallisticProjectileAimingProcess aiming_equip19_1770; //0x1770
    BallisticProjectileAimingProcess aimingGuillotineGun; //0x1970
    char pad_1B70[816]; //0x1B70
}; //Size: 0x1EA0
assert_offsetof(HasBallisticAimingEquipmentType, ballisticAimingCurrentEquipmentType, 0xD34);
assert_offsetof(HasBallisticAimingEquipmentType, aimingGuillotineGun, 0x1970);

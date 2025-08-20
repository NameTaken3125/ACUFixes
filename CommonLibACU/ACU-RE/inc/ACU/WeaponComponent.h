#pragma once

#include "ACU/Component.h"
#include "ACU/SoundEvent.h"
#include "ACU/SharedPtr.h"

class EntityAttributesComponent : public Component
{
public:
}; //Size: 0x0020
assert_sizeof(EntityAttributesComponent, 0x20);

class EquippableComponent : public EntityAttributesComponent
{
public:
    char pad_0020[16]; //0x0020
}; //Size: 0x0030
assert_sizeof(EquippableComponent, 0x30);
class NetFightWeapon_18
{
public:
    char pad_0000[28]; //0x0000
    uint32 maxAmmoInMagazine; //0x001C
    char pad_0020[4]; //0x0020
    float range; //0x0024
    char pad_0028[32]; //0x0028
}; //Size: 0x0048
class NetFightWeapon : public ManagedObject
{
public:
    char pad_0010[8]; //0x0010
    NetFightWeapon_18* magazineData; //0x0018
    char pad_0020[24]; //0x0020
}; //Size: 0x0038
class FirearmRemainingAmmoContainer
{
public:
    char pad_0000[16]; //0x0000
    void* shared_WeaponCollisionMaterialFXPack; //0x0010
    float float_18; //0x0018
    float float_1c; //0x001C
    uint32 ammoTillReload; //0x0020
    char pad_0024[92]; //0x0024
}; //Size: 0x0080

enum class WeaponType : uint32
{
    WeaponType_Unarmed = 0,
    WeaponType_Medium = 1,
    WeaponType_Small = 2,
    WeaponType_Heavy = 3,
    WeaponType_Long = 4,
    WeaponType_Musket = 5, // Rifle
    WeaponType_Blunderbuss = 6,
    WeaponType_Blunt = 7,
    WeaponType_HiddenBlade = 8,
    WeaponType_MediumBlunt = 9,
    WeaponType_Crossbow = 10,
    WeaponType_RightHolsterPistol = 11, // Pistol
    WeaponType_Bow = 12,
    WeaponType_ThrowingDagger = 13,
    WeaponType_Arrow = 14,
    WeaponType_Rock = 15,
    WeaponType_Gun = 16,
    WeaponType_AppleOfEden = 17,
    WeaponType_RopeDart = 18,
    WeaponType_LeftHolsterPistol = 19,
    WeaponType_Explosive = 20,
    WeaponType_LeftHandKnife = 21,
    WeaponType_SnitchDrum = 22,
    WeaponType_FreeAimGun = 23,
    WeaponType_WristCrossbow = 24, // PhantomBlade
    WeaponType_Bomb = 25,
    WeaponType_BeserkDart = 26, // BerserkBlade
    WeaponType_RiftMachineGun = 27,
    WeaponType_RiftCrossbow = 28,
    WeaponType_Mortar = 29, // GuillotineGun
    WeaponType_LanternDLC = 30,
    WeaponType_MaxWeaponType = 31,
};
enum class WeaponDesc
{
    WeaponDesc_None = 0,
    WeaponDesc_Pistol_Silenced = 1,
    WeaponDesc_Pistol_DoubleBarrel = 2,
    WeaponDesc_Pistol_DuckFoot = 3,
    WeaponDesc_Nightstick = 4,
    WeaponDesc_MAX = 5,
};
class WeaponComponent : public EquippableComponent
{
public:
    // @members
    char pad_0030[64]; //0x0030
    WeaponType weaponCpntType; //0x0070
    char pad_0074[4]; //0x0074
    SharedPtrNew<NetFightWeapon>* netFightWeapon; //0x0078
    char pad_0080[208]; //0x0080
    FirearmRemainingAmmoContainer* remainingAmmo; //0x0150
    char pad_0158[32]; //0x0158
    SoundInstance sound_178; //0x0178
    SoundInstance sound_1A8; //0x01A8
    char pad_01D8[8]; //0x01D8
    SoundInstance sound_1E0; //0x01E0

    // @members
    uint32 GetMaxAmmoInMagazine();
}; //Size: 0x0210
assert_sizeof(WeaponComponent, 0x210);

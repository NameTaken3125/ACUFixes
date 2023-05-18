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
    char pad_0020[40]; //0x0020
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
class WeaponComponent : public EquippableComponent
{
public:
    // @members
    char pad_0030[64]; //0x0030
    uint32 _2forGun; //0x0070
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

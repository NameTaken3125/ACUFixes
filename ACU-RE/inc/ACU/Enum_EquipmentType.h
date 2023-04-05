#pragma once

enum class EquipmentType : uint32
{
    unk_1 = 0x1,
    Pistol = 0x2,
    SmokeBomb = 0x5,
    MoneyPouch = 0xD,
    CherryBomb = 0x12,
    PoisonBomb = 0x13,
    StunBomb = 0x14,
    PhantomBlade = 0x16,
    BerserkBlade = 0x17,
    unk_0x19 = 0x19,
    Rifle = 0x1C,
    Blunderbuss = 0x1D,
    GuillotineGun = 0x1E,
    Slot9Booster = 0x20,
    MAX_mb = 37,
};
enum EquipmentTypeBitMask : uint64
{
    LeftHandWeapons = 0x70C00006,
};

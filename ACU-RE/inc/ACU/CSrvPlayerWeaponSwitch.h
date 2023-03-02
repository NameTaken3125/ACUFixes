#pragma once

#include "Statechart.h"
#include "Enum_EquipmentType.h"

class EntityAIProcess : public Statechart
{
public:
}; //Size: 0x00A8
assert_sizeof(EntityAIProcess, 0xA8);

class ESrvAbstract : public EntityAIProcess
{
public:
}; //Size: 0x00A8
assert_sizeof(ESrvAbstract, 0xA8);

class CSrvAbstract : public ESrvAbstract
{
public:
}; //Size: 0x00A8
assert_sizeof(CSrvAbstract, 0xA8);

class CSrvPlayerWeaponSwitch : public CSrvAbstract
{
public:
    char pad_00A8[8]; //0x00A8
    EquipmentType pendingSelectedEquipBombs; //0x00B0
    EquipmentType pendingSelectedEquipLeftHand; //0x00B4
    char pad_00B8[68]; //0x00B8
    EquipmentType equipType_LeftHand; //0x00FC
    EquipmentType equipType_bombs; //0x0100
    char pad_0104[4]; //0x0104
    uint32 dword_108; //0x0108
    uint32 dword_10c; //0x010C
    void* vtbl_110; //0x0110
}; //Size: 0x0118
assert_sizeof(CSrvPlayerWeaponSwitch, 0x118);

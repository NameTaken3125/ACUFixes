#pragma once

#include "ACU/ManagedObject.h"

enum class InventoryItemID : uint32
{
    SmokeBomb = 5,
    CherryBomb = 7,
    MoneyPouch = 8,
    PoisonGas = 0xA,
    Bullet = 0x12,
    PhantomBlade = 0x38,
    BerserkBlade = 0x39,
    MortarBomb = 0x44,
};

class InventoryItemCategorization : public Object
{
public:
    InventoryItemID invItemID; //0x0008
    uint32 categories_C; //0x000C
}; //Size: 0x0010
assert_sizeof(InventoryItemCategorization, 0x10);

class InventoryItemSettings : public ManagedObject
{
public:
    InventoryItemCategorization inventoryItemCategorization; //0x0010
    char pad_0020[24]; //0x0020
    uint8 isLocked; //0x0038
    char pad_0039[7]; //0x0039
}; //Size: 0x0040
assert_sizeof(InventoryItemSettings, 0x40);

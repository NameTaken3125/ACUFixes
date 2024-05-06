#pragma once

#include "ACU/ManagedObject.h"
#include "ACU/UIString.h"

enum class InventoryItemCategory : uint32
{
    SmokeBomb = 5,
    CherryBomb = 7,
    MoneyPouch = 8,
    PoisonGas = 0xA,
    Bullet = 0x12,
    PhantomBlade = 0x38,
    BerserkBlade = 0x39,
    MortarBomb = 0x44,

    Equipment = 0x37, // Belts, Chest, Head, Outfits, Colorsets etc. All non-weapon gears, it seems.
};

class InventoryItemCategorization : public Object
{
public:
    InventoryItemCategory Category; //0x0008
    uint32 SubCategory; //0x000C
}; //Size: 0x0010
assert_sizeof(InventoryItemCategorization, 0x10);

class InventoryItemSettings : public ManagedObject
{
public:
    // @members
    InventoryItemCategorization Categorization; //0x0010
    uint32 Source; //0x0020
    uint32 DefaultPrice; //0x0024
    UIString ItemName; //0x0028
    UIString ItemDescription; //0x002C
    uint32 Rank; //0x0030
    char pad_0034[4]; //0x0034
    uint8 Locked; //0x0038
    uint8 IsEconomicQuestItem; //0x0039
    uint8 Rechargeable; //0x003A
    uint8 StoreInInventory; //0x003B
    char pad_003C[4]; //0x003C

    // @helper_functions
    static TypeInfo& GetTI() { return *(TypeInfo*)0x143EE24F0; }
}; //Size: 0x0040
assert_sizeof(InventoryItemSettings, 0x40);

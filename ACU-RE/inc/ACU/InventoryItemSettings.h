#pragma once

#include "ACU/ManagedObject.h"
class InventoryItemCategorization : public Object
{
public:
    uint32 categories_8; //0x0008
    uint32 categories_C; //0x000C
}; //Size: 0x0010

class InventoryItemSettings : public ManagedObject
{
public:
    InventoryItemCategorization inventoryItemCategorization; //0x0010
    char pad_0020[24]; //0x0020
    uint8 isLocked; //0x0038
    char pad_0039[7]; //0x0039
}; //Size: 0x0040
assert_sizeof(InventoryItemSettings, 0x40);

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
    class InventoryItemCategorization inventoryItemCategorization; //0x0010
    char pad_0020[32]; //0x0020
}; //Size: 0x0040

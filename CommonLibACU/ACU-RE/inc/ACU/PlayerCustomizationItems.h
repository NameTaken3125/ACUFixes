#pragma once

#include "basic_types.h"
#include "InventoryItemSettings.h"
#include "SharedPtr.h"

class PlayerCustomizationItems
{
public:
	char pad_0000[48]; //0x0000
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_Pistol; //0x0030
	char pad_0038[16]; //0x0038
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_Colorset; //0x0048
	char pad_0050[48]; //0x0050
	uint8 byte_80; //0x0080
	char pad_0081[7]; //0x0081
}; //Size: 0x0088
assert_sizeof(PlayerCustomizationItems, 0x88);

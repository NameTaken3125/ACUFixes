#pragma once

#include "basic_types.h"
#include "InventoryItemSettings.h"
#include "SharedPtr.h"

class PlayerCustomizationItems
{
public:
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_Waist; //0x0000
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_Chest; //0x0008
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_Forearms; //0x0010
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_Head; //0x0018
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_Legs; //0x0020
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_WeaponOnehanded_mb; //0x0028
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_Pistol; //0x0030
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_LanternDLC; //0x0038
	char pad_0040[8]; //0x0040
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_Colorset; //0x0048
	char pad_0050[48]; //0x0050
	uint8 byte_80; //0x0080
	char pad_0081[7]; //0x0081
}; //Size: 0x0088
assert_sizeof(PlayerCustomizationItems, 0x88);

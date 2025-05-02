#pragma once

#include "basic_types.h"
#include "InventoryItemSettings.h"
#include "SharedPtr.h"

enum class AvatarSkillType : uint32
{
	AvatarSkillType_None = 0,
	AvatarSkillType_Morphing = 34,
};

class AvatarLoadout
{
public:
	SharedPtrNew<InventoryItemSettings>* WaistSlot; //0x0000
	SharedPtrNew<InventoryItemSettings>* ChestSlot; //0x0008
	SharedPtrNew<InventoryItemSettings>* ForearmsSlot; //0x0010
	SharedPtrNew<InventoryItemSettings>* HeadSlot; //0x0018
	SharedPtrNew<InventoryItemSettings>* LegsSlot; //0x0020
	SharedPtrNew<InventoryItemSettings>* MeleeWeaponSlot; //0x0028
	SharedPtrNew<InventoryItemSettings>* RangedWeaponSlot; //0x0030
	SharedPtrNew<InventoryItemSettings>* shared_invItemSett_LanternDLC; //0x0038
	SharedPtrNew<InventoryItemSettings>* OutfitSlot; //0x0040
	SharedPtrNew<InventoryItemSettings>* ColorSlot; //0x0048
	AvatarSkillType CurrentCoopSkill; //0x0050
	char pad_0054[4]; //0x0054
	SharedPtrNew<InventoryItemSettings>* CurrentSelectedBooster; //0x0058
	char pad_0060[32]; //0x0060
	uint8 byte_80; //0x0080
	char pad_0081[7]; //0x0081
}; //Size: 0x0088
assert_sizeof(AvatarLoadout, 0x88);

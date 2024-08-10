#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"

#include "InventoryItemSettings.h"

class NewPhysicalInventoryItem
{
public:
	// @members
	SharedPtrNew<InventoryItemSettings>* Item; //0x0000
	char pad_0008[8]; //0x0008

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x143EE2980; }
}; //Size: 0x0010
assert_sizeof(NewPhysicalInventoryItem, 0x10);

class CharacterAI;

class InventoryItemContainer : public Object
{
public:
	InventoryItemCategory Category; //0x0008
	SmallArray<NewPhysicalInventoryItem> ItemList; //0x000C
	SmallArray<NewPhysicalInventoryItem> OverflowItemList; //0x0018
	uint32 CurrentContainerLevel; //0x0024
	uint32 CurrentItemCount; //0x0028
	uint32 OverflowQuantity; //0x002C
	CharacterAI* charAI; //0x0030
	uint32 bonusAmountFromEquipment; //0x0038
	char pad_003C[4]; //0x003C
}; //Size: 0x0040
assert_sizeof(InventoryItemContainer, 0x40);

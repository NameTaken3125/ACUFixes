#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class InventoryItemSettings;

class AvatarBooster : public Object
{
public:
	char pad_0008[16]; //0x0008
	SharedPtrNew<InventoryItemSettings>* shared_invItemSettings; //0x0018
	char pad_0020[48]; //0x0020
}; //Size: 0x0050
assert_sizeof(AvatarBooster, 0x50);

class AvatarConsumable : public Object
{
public:
	SharedPtrNew<InventoryItemSettings>* shared_invItemSettings; //0x0018
	uint32 bitInAvatarSkillTree60; //0x0010
	char pad_0014[12]; //0x0014
	void* shared_TextureMap; //0x0020
	char pad_0028[12]; //0x0028
	uint32 oneItemCost; //0x0034
	char pad_0038[16]; //0x0038
}; //Size: 0x0048
assert_sizeof(AvatarConsumable, 0x48);

class AvatarConsumablesManager : public ManagedObject
{
public:
	// @members
	SmallArray<AvatarConsumable*> avatarConsumables; //0x0010
	SmallArray<AvatarBooster*> avatarBoosters; //0x001C
	char pad_0028[72]; //0x0028

	// @helper_functions
	static AvatarConsumablesManager* GetSingleton();
}; //Size: 0x0070
assert_sizeof(AvatarConsumablesManager, 0x70);

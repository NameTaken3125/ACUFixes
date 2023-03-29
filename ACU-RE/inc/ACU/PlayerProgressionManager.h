#pragma once

#include "MandatoryUniverseComponent.h"
#include "SmallArray.h"
#include "SharedPtr.h"
#include "AIAction.h"

class SinglePlayerOptions
{
public:
	char pad_0000[32]; //0x0000
}; //Size: 0x0020
assert_sizeof(SinglePlayerOptions, 0x20);
class PlayerOptions : public ManagedObject
{
public:
	char pad_0010[8]; //0x0010
	SinglePlayerOptions singlePlayerOptions; //0x0018
	char pad_0038[24]; //0x0038
}; //Size: 0x0050
assert_sizeof(PlayerOptions, 0x50);
class WorldUpgradeInventorySettingsRepository : public Object
{
public:
	char pad_0008[16]; //0x0008
}; //Size: 0x0018
assert_sizeof(WorldUpgradeInventorySettingsRepository, 0x18);
class InventoryItemQuestsRepository : public Object
{
public:
	char pad_0008[16]; //0x0008
}; //Size: 0x0018
assert_sizeof(InventoryItemQuestsRepository, 0x18);

class InventoryItemCategoryDataRepository : public Object
{
public:
	char pad_0008[16]; //0x0008
}; //Size: 0x0018
assert_sizeof(InventoryItemCategoryDataRepository, 0x18);

class InventoryItemDataRepository : public Object
{
public:
	char pad_0008[16]; //0x0008
}; //Size: 0x0018
assert_sizeof(InventoryItemDataRepository, 0x18);


class ActionNotoriety : public AIAction
{
public:
	char pad_0040[32]; //0x0040
}; //Size: 0x0060
assert_sizeof(ActionNotoriety, 0x60);

class InventoryItemSettings;
class PlayerProgressionSaveData : public Object
{
public:
	SmallArray<InventoryItemSettings*> papInventoryItemSettings; //0x0008
	char pad_0014[76]; //0x0014
}; //Size: 0x0060
assert_sizeof(PlayerProgressionSaveData, 0x60);

class PlayerRankDetails;
class PlayerRanker : public ManagedObject
{
public:
	char pad_0010[8]; //0x0010
	PlayerRankDetails* playerRankDetails; //0x0018
	char pad_0020[48]; //0x0020
}; //Size: 0x0050
assert_sizeof(PlayerRanker, 0x50);

class MissionStep : public ManagedObject
{
public:
	char pad_0010[144]; //0x0010
}; //Size: 0x00A0
assert_sizeof(MissionStep, 0xA0);

class MissionStepCollectible : public MissionStep
{
public:
	char pad_00A0[88]; //0x00A0
}; //Size: 0x00F8
assert_sizeof(MissionStepCollectible, 0xF8);

class PlayerCharacterSettings : public Object
{
public:
	char pad_0008[56]; //0x0008
}; //Size: 0x0040
assert_sizeof(PlayerCharacterSettings, 0x40);

class AssassinAbilitySet;

class AssassinAbilitySets
{
public:
	AssassinAbilitySet* assassinAbilitySet; //0x0000
	SmallArray<AssassinAbilitySet*> additionalAbilitySets; //0x0008
	uint8 isUsingArray_mb; //0x0014
	char pad_0015[43]; //0x0015

	// @helper_functions
	AssassinAbilitySet* GetLowestPrioritySet() { return assassinAbilitySet; }
	AssassinAbilitySet* GetHighestPrioritySet()
	{
		// Implementation at e.g. 140B9C4D0
		return additionalAbilitySets.size
			? additionalAbilitySets[additionalAbilitySets.size - 1]
			: assassinAbilitySet;
	}
}; //Size: 0x0040
assert_sizeof(AssassinAbilitySets, 0x40);


class ProgressionCharacter : public ManagedObject
{
public:
	char* str_Arno_MainCharacter; //0x0010
}; //Size: 0x0018
assert_sizeof(ProgressionCharacter, 0x18);


class InventoryItemList : public Object
{
public:
	char pad_0008[16]; //0x0008
}; //Size: 0x0018

class PlayerProgressionCharacterData_190
{
public:
	char pad_0000[48]; //0x0000
}; //Size: 0x0030

class PlayerProgressionCharacterData
{
public:
	char pad_0000[40]; //0x0000
	InventoryItemList invItemList_28; //0x0028
	InventoryItemList invItemList_40; //0x0040
	InventoryItemList invItemList_58; //0x0058
	InventoryItemList invItemList_70; //0x0070
	char pad_0088[12]; //0x0088
	SmallArray<SharedPtrNew<InventoryItemSettings>*> invItemSettings_94; //0x0094
	SharedPtrNew<ProgressionCharacter>* shared_progressionCharacter; //0x00A0
	char pad_00A8[232]; //0x00A8
	PlayerProgressionCharacterData_190 stru_190; //0x0190
	char pad_01C0[128]; //0x01C0
}; //Size: 0x0240
assert_offsetof(PlayerProgressionCharacterData, stru_190, 0x190);
assert_sizeof(PlayerProgressionCharacterData, 0x240);

class PlayerProgressionManager : public MandatoryUniverseComponent
{
public:
	// @members
	char pad_0010[48]; //0x0010
	PlayerOptions playerOptions; //0x0040
	char pad_0090[8]; //0x0090
	WorldUpgradeInventorySettingsRepository worldUpgradeInventorySettingsRepository; //0x0098
	InventoryItemQuestsRepository inventoryItemQuestsRepository; //0x00B0
	char pad_00C8[24]; //0x00C8
	InventoryItemCategoryDataRepository inventoryItemCategoryDataRepository; //0x00E0
	InventoryItemDataRepository inventoryItemDataRepository; //0x00F8
	char pad_0110[16]; //0x0110
	ActionNotoriety actionNotoriety; //0x0120
	SmallArray<PlayerCharacterSettings*> papPlayerCharacterSettings; //0x0180
	char pad_018C[12]; //0x018C
	SharedPtrNew<ProgressionCharacter>* ppProgressionCharacter_198; //0x0198
	SmallArray<PlayerProgressionCharacterData*> papPlayerProgressionCharacterData; //0x01A0
	char pad_01AC[12]; //0x01AC
	SharedPtrNew<ProgressionCharacter>* ppProgressionCharacter_1B8; //0x01B8
	SharedPtrNew<Entity>* sharedPtr_player; //0x01C0
	SmallArray<PlayerProgressionCharacterData*> papPlayerProgressionCharacterData_1C8; //0x01C8
	char pad_01D4[4]; //0x01D4
	PlayerProgressionSaveData playerProgressionSaveData; //0x01D8
	char pad_0238[16]; //0x0238
	PlayerRanker playerRanker; //0x0248
	char pad_0298[40]; //0x0298
	InventoryItemCategoryDataRepository inventoryItemCategoryDataRepository_2C0; //0x02C0
	InventoryItemDataRepository inventoryItemDataRepository_2D8; //0x02D8
	char pad_02F0[64]; //0x02F0
	MissionStepCollectible missionStepCollectible; //0x0330
	char pad_0428[128]; //0x0428
	PlayerCharacterSettings playerCharacterSettings_4A8; //0x04A8
	AssassinAbilitySets assassinAbilitySets; //0x04E8
	char pad_0528[72]; //0x0528

	// @helper_functions
	static PlayerProgressionManager* GetSingleton() { return *(PlayerProgressionManager**)0x1451B3C58; }
}; //Size: 0x0570
assert_offsetof(PlayerProgressionManager, assassinAbilitySets, 0x4E8);
assert_sizeof(PlayerProgressionManager, 0x570);

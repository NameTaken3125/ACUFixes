#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "HUDModules.h"

enum class HUDModes : uint32
{
	HUDModes_Menu = 0,
	HUDModes_Cinematics = 1,
	HUDModes_Game = 2,
	HUDModes_Desync = 3,
	HUDModes_MissionTransition = 4,
	HUDModes_MissionBriefing = 5,
	HUDModes_MissionRecap = 6,
	HUDModes_MissionRecapRiftLGS = 7,
	HUDModes_Loading = 8,
	HUDModes_LockPick = 9,
	HUDModes_Custom = 10,
	HUDModes_TrophyDisplay = 11,
	HUDModes_TitleScreen = 12,
	HUDModes_ReachHighPoint = 13,
	HUDModes_BlackBox = 14,
	HUDModes_HudPanel = 15,
	HUDModes_Startup = 16,
	HUDModes_Count = 17,
	HUDModes_Invalid = 18,
};

class UIHUDModeEntry : public ManagedObject
{
public:
	HUDModes Mode; //0x0010
	SmallArray<HUDModules> Modules; //0x0014
	uint32 NameCRC32; //0x0020
	uint8 IsValid; //0x0024
	char pad_0025[3]; //0x0025
}; //Size: 0x0028
assert_sizeof(UIHUDModeEntry, 0x28);

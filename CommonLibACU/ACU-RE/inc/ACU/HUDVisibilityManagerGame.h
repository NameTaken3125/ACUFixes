#pragma once

#include "UniverseComponent.h"
#include "SmallArray.h"
#include "HUDModules.h"

class UIHUDModeEntry;

class HUDVisibilityManagerGame : public UniverseComponent
{
public:
	char pad_0010[8]; //0x0010
	SmallArray<UIHUDModeEntry*> m_UIHUDModeEntries; //0x0018
	SmallArray<HUDModules> m_HUDAllHiddenAdditionnalModules; //0x0024
	SmallArray<HUDModules> m_HUDLightHiddenAdditionnalModules; //0x0030
	char pad_003C[364]; //0x003C

	static HUDVisibilityManagerGame* GetSingleton() { return *(HUDVisibilityManagerGame**)0x145196D40; }
}; //Size: 0x01A8
assert_sizeof(HUDVisibilityManagerGame, 0x1A8);

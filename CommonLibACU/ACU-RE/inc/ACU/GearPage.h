#pragma once

#include "StandardPage.h"
#include "AvatarLoadout.h"
#include "PlayerCustomizationWorker.h"
#include "SharedPtr.h"

class UIPositionTweener : public Object
{
public:
	char pad_0008[120]; //0x0008
}; //Size: 0x0080
assert_sizeof(UIPositionTweener, 0x80);

class PanelPageHelper;

class GearPage : public StandardPage
{
public:
	char pad_0088[48]; //0x0088
	PlayerCustomizationWorker customizationWorker; //0x00B8
	uint8 isNeedsUpdate; //0x03D8
	char pad_03D9[199]; //0x03D9
	PanelPageHelper* panelPageHelper; //0x04A0
	char pad_04A8[0x5B0-0x4A8]; //0x04A8
	UIPositionTweener uiPositionTweener; //0x05B0
	char pad_0630[144]; //0x0630
}; //Size: 0x06C0
assert_sizeof(GearPage, 0x6C0);

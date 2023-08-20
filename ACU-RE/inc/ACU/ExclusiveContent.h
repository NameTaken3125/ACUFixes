#pragma once

#include "basic_types.h"
#include "SmallArray.h"
#include "UIString.h"

class ExclusiveContent : public Object
{
public:
	uint32 dword_8; //0x0008
	char pad_000C[4]; //0x000C
	wchar_t* str10; //0x0010
	UIString uiString_Name; //0x0018
	UIString uiString_Description; //0x001C
	uint8 byte_20_isUnlocked_mb; //0x0020
	char pad_0021[7]; //0x0021
}; //Size: 0x0028
assert_sizeof(ExclusiveContent, 0x28);

class UsedWhenCheckingIfMissionCompleted
{
public:
	// @members
	char pad_0000[352]; //0x0000
	SmallArray<ExclusiveContent> arrExclusiveContent; //0x0160
	char pad_016C[4]; //0x016C

	// @helper_functions
	static UsedWhenCheckingIfMissionCompleted* GetSingleton() { return *(UsedWhenCheckingIfMissionCompleted**)0x145198478; }
}; //Size: 0x0170
assert_offsetof(UsedWhenCheckingIfMissionCompleted, arrExclusiveContent, 0x160);
assert_sizeof(UsedWhenCheckingIfMissionCompleted, 0x170);

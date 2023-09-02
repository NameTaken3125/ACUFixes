#pragma once

#include "HUDModule.h"

class HUDQuickSelectModule : public HUDModule
{
public:
	char pad_0090[328]; //0x0090
	uint64 prevPressTimestamp_mb; //0x01D8
	uint64 lastPressTimestamp_mb; //0x01E0
	char pad_01E8[32]; //0x01E8
}; //Size: 0x0208
assert_sizeof(HUDQuickSelectModule, 0x208);

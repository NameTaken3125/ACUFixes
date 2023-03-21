#pragma once

#include "CSrvAbstract.h"

class CSrvHealth : public CSrvAbstract
{
public:
	char pad_00A8[32]; //0x00A8
	class FallDamage* fallDamages; //0x00C8
	char pad_00D0[30]; //0x00D0
	uint8 bGodmode; //0x00EE
	char pad_00EF[161]; //0x00EF
}; //Size: 0x0190
assert_sizeof(CSrvHealth, 0x190);

class CSrvPlayerHealth : public CSrvHealth
{
public:
	char pad_0190[148]; //0x0190
	uint32 health; //0x0224
	uint32 maxHealth; //0x0228
	uint8 isDesynchronizationNow; //0x022C
	char pad_022D[75]; //0x022D
}; //Size: 0x0278
assert_offsetof(CSrvPlayerHealth, isDesynchronizationNow, 0x22C);
assert_sizeof(CSrvPlayerHealth, 0x278);

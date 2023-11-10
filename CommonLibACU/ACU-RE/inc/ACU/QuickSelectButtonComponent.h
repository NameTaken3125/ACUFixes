#pragma once

#include "Enum_EquipmentType.h"
#include "ButtonComponent.h"

class EntityGroup;

class QuickSelectButtonComponent : public ButtonComponent
{
public:
	EquipmentType equipmentType; //0x02A0
	char pad_02A4[36]; //0x02A4
	uint32 dword_2c8; //0x02C8
	char pad_02CC[4]; //0x02CC
}; //Size: 0x02D0
assert_offsetof(QuickSelectButtonComponent, equipmentType, 0x02A0);
assert_sizeof(QuickSelectButtonComponent, 0x02D0);

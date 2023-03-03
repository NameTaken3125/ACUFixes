#pragma once

#include "DataBindingComponent.h"
#include "Enum_EquipmentType.h"

class EntityGroup;

class WidgetComponent : public DataBindingComponent
{
public:
	char pad_0098[162]; //0x0098
	uint8 isSlotHighlighted; //0x013A
	char pad_013B[157]; //0x013B
	EntityGroup* entityGroup_1d8; //0x01D8
	char pad_01E0[48]; //0x01E0
}; //Size: 0x0210
assert_offsetof(WidgetComponent, isSlotHighlighted, 0x13A);
assert_sizeof(WidgetComponent, 0x210);

class ButtonComponent : public WidgetComponent
{
public:
	char pad_0210[144]; //0x0210
}; //Size: 0x02A0
assert_sizeof(ButtonComponent, 0x02A0);

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

#pragma once

#include "SmallArray.h"
#include "DataBindingComponent.h"

class WidgetComponent : public DataBindingComponent
{
public:
	char pad_0098[162]; //0x0098
	uint8 isSlotHighlighted; //0x013A
	char pad_013B[5]; //0x013B
	uint8 isNotGreyedOut_mb; //0x0140
	char pad_0141[151]; //0x0141
	class EntityGroup* entityGroup_1d8; //0x01D8
	char pad_01E0[48]; //0x01E0
}; //Size: 0x0210
assert_offsetof(WidgetComponent, isSlotHighlighted, 0x13A);
assert_offsetof(WidgetComponent, isNotGreyedOut_mb, 0x140);
assert_sizeof(WidgetComponent, 0x210);

class ButtonComponent_230
{
public:
	// @members
	void* callbackAs1stParam; //0x0000
	char pad_0008[8]; //0x0008
	void* receiverAndFlags; //0x0010
	void* callback_mb; //0x0018

	// @helper_functions
	void* GetReceiver() { return (void*)(((uint64)receiverAndFlags) & 0xFFFFFFFFFFFFFFFCu); }
}; //Size: 0x0020
assert_sizeof(ButtonComponent_230, 0x20);

class ButtonComponent : public WidgetComponent
{
public:
	char pad_0210[32]; //0x0210
	SmallArray<ButtonComponent_230> arrCallbacksWhenEnabled_mb; //0x0230
	SmallArray<ButtonComponent_230> arrCallbacksWhenDisabled_mb; //0x023C
	char pad_0248[88]; //0x0248
}; //Size: 0x02A0
assert_offsetof(ButtonComponent, arrCallbacksWhenEnabled_mb, 0x230);
assert_sizeof(ButtonComponent, 0x02A0);

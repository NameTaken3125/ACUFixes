#pragma once

#include "DataBindingComponent.h"

class UIComponent : public DataBindingComponent
{
public:
	char pad_0098[59]; //0x0098
	uint8 isVisible_d3; //0x00D3
	char pad_00D4[1]; //0x00D4
	uint8 isVisible_d5; //0x00D5
	char pad_00D6[10]; //0x00D6
}; //Size: 0x00E0
assert_sizeof(DataBindingComponent, 0x98);

class SharedPtr_mb;
class UIEventReaction;
class QuickSelectButtonComponent;

class HUDQuickSelectComponent : public UIComponent
{
public:
	char pad_00E0[48]; //0x00E0
	SmallArray<SharedPtr_mb> equipmentSlots; //0x0110
	char pad_011C[36]; //0x011C
	void* uiEventReactions_140; //0x0140
	char pad_0148[40]; //0x0148
	SmallArray<UIEventReaction*> uiEventReactions_170; //0x0170
	char pad_017C[12]; //0x0178
	QuickSelectButtonComponent* quickSelectButtonComponent_188_pendingSelection; //0x0188
	char pad_0190[14]; //0x0190
	uint8 isSelectedEquipmentLocked; //0x019E
	char pad_019F[57]; //0x019F
	QuickSelectButtonComponent* quickSelectButtonComponent_1D8_readOnceOnNumpad46; //0x01D8
	char pad_01E0[24]; //0x01E0
	uint32 dword_1f8; //0x01F8
	char pad_01FC[4]; //0x01FC
}; //Size: 0x0200
assert_offsetof(HUDQuickSelectComponent, quickSelectButtonComponent_188_pendingSelection, 0x188);
assert_sizeof(HUDQuickSelectComponent, 0x0200);

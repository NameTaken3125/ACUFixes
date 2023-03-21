#pragma once

#include "ManagedObject.h"

class AbilitySet : public ManagedObject
{
public:
}; //Size: 0x0010
assert_sizeof(AbilitySet, 0x10);

class AssassinAbilitySet : public AbilitySet
{
public:
	struct Flags
	{
		char pad_0010[8]; //0x0010

		bool unk18_0 : 1;
		bool unk18_1 : 1;
		bool unk18_2 : 1;
		bool unk18_3 : 1;
		bool unk18_4 : 1;
		bool unk18_5 : 1;
		bool unk18_6 : 1;
		bool unk18_7 : 1;
		bool unk18_8 : 1;
		bool unk18_9 : 1;
		bool unk18_A : 1;
		bool unk18_B : 1;
		bool unk18_C : 1;
		bool allowEnterClosets : 1;
		bool unk18_E : 1;
		bool unk18_F : 1;

		char pad1E[6];

		bool allowWASD : 1;
		bool allowCamera_sometimes : 1;
		bool unk20_2 : 1;
		bool unk20_3 : 1;
		bool unk20_4 : 1;
		bool unk20_5 : 1;
		bool unk20_6 : 1;
		bool unk20_7 : 1;
		bool allowWallMovement : 1;
		bool unk20_9 : 1;
		bool unk20_A : 1;
		bool unk20_B : 1;
		bool allowLadders : 1;
		bool unk20_D : 1;
		bool unk20_E : 1;
		bool unk20_F : 1;

		char pad22[6];
		char pad_0028[8]; //0x0028
		uint32 flags30; //0x0030

		bool allowSprint_mb : 1;
		bool unk34_1 : 1;
		bool unk34_2 : 1;
		bool allowPlayerAndCameraControls : 1;
		bool unk34_4 : 1;
		bool unk34_5 : 1;
		bool unk34_6 : 1;
		bool unk34_7 : 1;
		char pad_0035[3]; //0x0035
	} flags;
}; //Size: 0x0038
assert_sizeof(AssassinAbilitySet, 0x38);

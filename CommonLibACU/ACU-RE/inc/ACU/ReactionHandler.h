#pragma once

#include "Statechart.h"

class Entity;
class WitnessEvent;
class AlertedParams;
class ReactionPack;


class ReactionHandler_148
{
public:
	char pad_0000[8]; //0x0000
	WitnessEvent* witnessEvn8; //0x0008
	char pad_0010[8]; //0x0010
	void* p18; //0x0018
	char pad_0020[8]; //0x0020
	WitnessEvent* witnessEvn28; //0x0028
	char pad_0030[8]; //0x0030
	void* shared_38; //0x0038
	ReactionPack** reactionPack; //0x0040
	char pad_0048[208]; //0x0048
	AlertedParams* pushReactionParams_118; //0x0118
	void* pushReactionParams_120; //0x0120
	char pad_0128[8]; //0x0128
}; //Size: 0x0130

class ReactionHandler_418
{
public:
	char pad_0000[88]; //0x0000
	ReactionPack** shared_ReactionPack; //0x0058
}; //Size: 0x0060

class ReactionHandler : public Statechart
{
public:
	char pad_00A8[24]; //0x00A8
	void* shared_C0; //0x00C0
	char pad_00C8[112]; //0x00C8
	Entity* thisEntity; //0x0138
	CharacterBehavior* thisCharacterBehavior; //0x0140
	ReactionHandler_148 stru148; //0x0148
	AlertedParams* alertedParams_278; //0x0278
	char pad_0280[353]; //0x0280
	uint8 byte_3E1; //0x03E1
	char pad_03E2[54]; //0x03E2
	ReactionHandler_418 stru_418; //0x0418
	char pad_0478[40]; //0x0478
}; //Size: 0x04A0
assert_offsetof(ReactionHandler, stru148, 0x148);
assert_offsetof(ReactionHandler, stru_418, 0x418);
assert_sizeof(ReactionHandler, 0x4A0);

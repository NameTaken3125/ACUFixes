#pragma once

#include "WorldComponent.h"

class Channel : public ManagedObject
{
public:
	void* shared_EventListener; //0x0010
	char pad_0018[32]; //0x0018
}; //Size: 0x0038
assert_sizeof(Channel, 0x38);

class VanishingManager : public WorldComponent
{
public:
	// @members
	uint32 _8permanentBlend2crowdBlend0x40moneyPouch; //0x0020
	char pad_0024[20]; //0x0024
	Channel channel38; //0x0038
	char pad_0070[64]; //0x0070

	// @helper_functions
	static VanishingManager* GetSingleton();
}; //Size: 0x00B0
assert_offsetof(VanishingManager, _8permanentBlend2crowdBlend0x40moneyPouch, 0x20);
assert_sizeof(VanishingManager, 0xB0);

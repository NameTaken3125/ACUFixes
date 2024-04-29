#pragma once

#include "ManagedObject.h"
#include "AvatarLoadout.h"

class AvatarGear;
class Entity;
class SpawningSpecification;

class PlayerCustomizationWorker : public ManagedObject
{
public:
	char pad_0010[408]; //0x0010
	uint8 byte_1A8; //0x01A8
	char pad_01A9[23]; //0x01A9
	AvatarLoadout currentCustomizationItems; //0x01C0
	char pad_0248[1]; //0x0248
	uint8 byte_249; //0x0249
	uint8 byte_24A; //0x024A
	char pad_024B[5]; //0x024B
	SpawningSpecification* spawningSpecification; //0x0250
	SharedPtrNew<Entity>* shared_fakePlayer_mb; //0x0258
	AvatarGear* newHighlightedGear; //0x0260
	char pad_0268[8]; //0x0268
	AvatarGear* previouslyHighlightedGear; //0x0270
	char pad_0278[168]; //0x0278
}; //Size: 0x0320
assert_offsetof(PlayerCustomizationWorker, currentCustomizationItems, 0x1C0);
assert_offsetof(PlayerCustomizationWorker, newHighlightedGear, 0x260);
assert_offsetof(PlayerCustomizationWorker, previouslyHighlightedGear, 0x270);
assert_sizeof(PlayerCustomizationWorker, 0x320);

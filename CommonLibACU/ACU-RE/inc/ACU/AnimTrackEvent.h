#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class AnimTrack : public Object
{
public:
	void* KeyTimes; //0x0008
	uint16 numKeyTimes; //0x0010
	char pad_0012[6]; //0x0012
}; //Size: 0x0018
assert_sizeof(AnimTrack, 0x18);

class EventSeed;

class AnimTrackEvent : public AnimTrack
{
public:
	SmallArray<EventSeed*> KeyAnimEvents; //0x0018
	char pad_0024[4]; //0x0024
	uint16 BodyPartMask; //0x0028
	char pad_002A[6]; //0x002A
}; //Size: 0x0030
assert_sizeof(AnimTrackEvent, 0x30);

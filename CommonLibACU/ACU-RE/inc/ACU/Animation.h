#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class ObjectAllocator : public ManagedObject
{
public:
	char pad_0010[8]; //0x0010
}; //Size: 0x0018
assert_sizeof(ObjectAllocator, 0x18);

class AnimTrackEvent;


class AnimTrackDataMapping
{
public:
	uint32 TrackID; //0x0000
}; //Size: 0x0004
assert_sizeof(AnimTrackDataMapping, 4);
class Animation;
class AnimTrackData
{
public:
	uint32 AnimTrackDataKey; //0x0000
	uint16 NumRefs; //0x0004
	SmallArray<AnimTrackDataMapping> AnimTrackDataMapping_; //0x0006
	char pad_0012[6]; //0x0012
	Animation* animation; //0x0018
}; //Size: 0x0020
assert_sizeof(AnimTrackData, 0x20);

class Animation : public ObjectAllocator
{
public:
	// @members
	SmallArray<uint16> TeleportAtTime; //0x0018
	SmallArray<void*> arr24; //0x0024
	SmallArray<AnimTrackEvent*> EventsTracks; //0x0030
	char pad_003C[4]; //0x003C
	AnimTrackData* AnimTrackData_; //0x0040
	float Length; //0x0048
	uint32 AnimationKey; //0x004C

	bool Additive : 1; //0x0050
	bool AdditiveWithAlwaysMatchBlendLength : 1;
	bool UseGlobalCoordsForReferenceBone : 1;
	bool IsStreamed : 1;

	char pad_0051[15]; //0x0051
	SmallArray<void*> rawTracks; //0x0060
	char pad_006C[4]; //0x006C

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439CD110; }
	static constexpr uint64 GetVTBL() { return 0x142E7ACC0; }
}; //Size: 0x0070
assert_sizeof(Animation, 0x70);

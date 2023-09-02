#pragma once

#include "SoundResource.h"
class GroupSoundResource : public SoundResource
{
public:
	char pad_0010[8]; //0x0010
}; //Size: 0x0018
assert_sizeof(GroupSoundResource, 0x18);

class SoundState : public GroupSoundResource
{
public:
}; //Size: 0x0018
assert_sizeof(SoundState, 0x18);

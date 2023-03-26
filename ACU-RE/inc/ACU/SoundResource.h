#pragma once

#include "ManagedObject.h"

class SoundID
{
public:
	uint32 soundEvnHash; //0x0000
}; //Size: 0x0004
assert_sizeof(SoundID, 4);

class SoundResource : public Object
{
public:
	class SoundID soundID; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
assert_sizeof(SoundResource, 0x10);

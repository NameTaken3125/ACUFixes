#pragma once

#include "basic_types.h"

class LoadInfo
{
public:
	char* FileName; //0x0000
	uint64 ObjectID; //0x0008
	uint32 FileClassID; //0x0010
	char pad_0014[4]; //0x0014
}; //Size: 0x0018
assert_sizeof(LoadInfo, 0x18);

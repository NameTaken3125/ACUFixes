#pragma once

#include "basic_types.h"

class TagDescriptor
{
public:
	char* TagName; //0x0000
	uint32 Tag; //0x0008
	uint32 GroupTag; //0x000C
	uint16 UseDeepSearchesInTables : 1; //0x0010
	uint16 __flags_mb : 15;
	uint16 thisTagIdx; //0x0012
	char pad_0014[4]; //0x0014
}; //Size: 0x0018
assert_sizeof(TagDescriptor, 0x18);

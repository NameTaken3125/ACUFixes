#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class BuildTag
{
public:
	char** ppstr; //0x0000
}; //Size: 0x0008
assert_sizeof(BuildTag, 8);

class BuildTags
{
public:
	SmallArray<BuildTag> tags; //0x0000
}; //Size: 0x000C
assert_sizeof(BuildTags, 0xC);

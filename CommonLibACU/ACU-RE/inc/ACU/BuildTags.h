#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class TagDescriptor;

class BuildTag
{
public:
    // @members
    TagDescriptor* EngineTagSave; //0x0000

    // @helper_functions
    BuildTag() : EngineTagSave(nullptr) {}
    BuildTag(TagDescriptor& tag) : EngineTagSave(&tag) {}
}; //Size: 0x0008
assert_sizeof(BuildTag, 8);

class BuildTags
{
public:
    SmallArray<BuildTag> tags; //0x0000
}; //Size: 0x000C
assert_sizeof(BuildTags, 0xC);

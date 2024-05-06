#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"
#include "BuildTags.h"

class TagRuleRetag
{
public:
	uint32 RemoveMatchedTag : 1;
	uint32 AllowRandomSelection : 1;
	uint32 WeightForRandomSelection : 30;
	SmallArray<BuildTag> Tags; //0x0004
	BuildTag TagToMatch; //0x0010
}; //Size: 0x0018
assert_sizeof(TagRuleRetag, 0x18);

class BuildTable;

class TagRuleBuildTable
{
public:
	SharedPtrNew<BuildTable>* Table; //0x0000
	SharedPtrNew<BuildTable>* TableHandle; //0x0008
	uint8 IsMainBuilder; //0x0010
	uint8 LoadOnDemand_unkBit1; //0x0011
	char pad_0012[6]; //0x0012
	class BuildTag TagToMatch; //0x0018
}; //Size: 0x0020
assert_sizeof(TagRuleBuildTable, 0x20);

class TagRuleSelector;
class EntityBuilder;

class TagRules : public ManagedObject
{
public:
	SmallArray<TagRuleRetag> Retags; //0x0010
	SmallArray<TagRuleSelector> Selectors; //0x001C
	SmallArray<TagRuleBuildTable> Builders; //0x0028
	char pad_0034[4]; //0x0034
	SharedPtrNew<EntityBuilder>* DefaultBuilder; //0x0038
	char pad_0040[8]; //0x0040
}; //Size: 0x0048
assert_sizeof(TagRules, 0x48);

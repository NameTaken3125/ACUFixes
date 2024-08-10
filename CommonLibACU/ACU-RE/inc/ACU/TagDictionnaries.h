#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"
#include "BuildTags.h"



class TagGroupDescriptor
{
public:
	uint32 GroupTag; //0x0000
	uint8 Exclusive; //0x0004
	BuildTags Tags; //0x0005
	char pad_0011[3]; //0x0011
}; //Size: 0x0014
assert_sizeof(TagGroupDescriptor, 0x14);

class TagDescriptor;

class TagDictionnary : public ManagedObject
{
public:
	SmallArray<TagDescriptor> Tags; //0x0010
	char pad_001C[4]; //0x001C
}; //Size: 0x0020
assert_sizeof(TagDictionnary, 0x20);

class TagDictionnaries : public ManagedObject
{
public:
	// @members
	SmallArray<SharedPtrNew<TagDictionnary>*> Dictionnaries; //0x0010
	SmallArray<TagGroupDescriptor> Groups; //0x001C
	void* hashmap_tagDescriptorByTagDword; //0x0028
	char pad_0030[8]; //0x0030

	// @helper_functions
	static TagDictionnaries* GetSingleton() { return *(TagDictionnaries**)0x145217370; }
}; //Size: 0x0038
assert_sizeof(TagDictionnaries, 0x38);

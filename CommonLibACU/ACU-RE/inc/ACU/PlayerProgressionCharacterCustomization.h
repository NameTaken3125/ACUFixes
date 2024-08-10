#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "BuildTags.h"
#include "SharedPtr.h"

class BodyCustomizablePart;
class InventoryItemSettings;

class BodyBuildTag : public ManagedObject
{
public:
	// @members
	BodyCustomizablePart* bodyCustomizablePart; //0x0010
	char* str; //0x0018
	SharedPtrNew<InventoryItemSettings>* shared_invItem; //0x0020
	BuildTags Tags; //0x0028
	uint8 isCurrentOrIsLoaded_mb; //0x0034
	char pad_0035[3]; //0x0035

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x143E37430; }
}; //Size: 0x0038
assert_sizeof(BodyBuildTag, 0x38);

class BodyCustomizablePart : public Object
{
public:
	SmallArray<BodyBuildTag*> BodyBuildTag_; //0x0008
	BuildTags DefaultTags; //0x0014
}; //Size: 0x0020
assert_sizeof(BodyCustomizablePart, 0x20);


class BodyBaseVisualType : public ManagedObject
{
public:
	SmallArray<BodyCustomizablePart*> bodyCustomizableParts; //0x0010
	char pad_001C[4]; //0x001C
}; //Size: 0x0020
assert_sizeof(BodyBaseVisualType, 0x20);

class PlayerProgressionCharacterCustomization : public ManagedObject
{
public:
	SmallArray<BodyBaseVisualType*> arrBodyBaseVisualType; //0x0010
	char pad_001C[4]; //0x001C
}; //Size: 0x0020
assert_sizeof(PlayerProgressionCharacterCustomization, 0x20);

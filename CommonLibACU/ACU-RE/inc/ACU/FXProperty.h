#pragma once

#include "basic_types.h"

enum class FXInputMode : uint32
{
	FXINPUTMODE_ABSOLUTE = 0,
};

enum class FXPropertyType : uint32
{
	FXPROPTYPE_INTEGER = 2,
};

enum class FXBlendMode : uint32
{
	FXBLENDMODE_NONE = 0,
};

#include "PropertyPathNode.h"
#include "SmallArray.h"

class PropertyPath
{
public:
	uint8 TargetMustBeUnique; //0x0000
	uint8 WholeArray; //0x0001
	SmallArray<PropertyPathNode> PathNodes; //0x0002
	char pad_000E[10]; //0x000E
}; //Size: 0x0018
assert_sizeof(PropertyPath, 0x18);

class FXProperty
{
public:
	FXInputMode InputMode : 3;
	uint32 MustRestoreValue : 1;
	uint32 MustClampValue : 1;
	uint32 MustSetValueOnce : 1;
	FXPropertyType FXPropertyType_ : 3;
	FXBlendMode BlendInMode : 3;
	FXBlendMode BlendOutMode : 3;
	uint32 RegisterIndex : 11;
	float StartTime;
	float EndTime;
	PropertyPath PropertyPath_; //0x000C
	char pad_0024[4]; //0x0024
	void* DynamicProperties; //0x0028
	char pad_0030[24]; //0x0030
};
assert_sizeof(FXProperty, 0x48);
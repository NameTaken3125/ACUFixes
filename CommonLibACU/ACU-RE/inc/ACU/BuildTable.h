#pragma once

#include "vmath/vmath.h"
#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"
#include "BuildTags.h"

class BuildColumn_Component_DynamicProperty
{
public:
	char pad_0000[4]; //0x0000
	uint32 Index_orNameHash; //0x0004
	uint32 DataType; //0x0008
	uint32 Type; //0x000C
	char pad_0010[40]; //0x0010
}; //Size: 0x0038
assert_sizeof(BuildColumn_Component_DynamicProperty, 0x38);
class BuildColumn_Components
{
public:
	char pad_0000[16]; //0x0000
	SmallArray<BuildColumn_Component_DynamicProperty> cpnts; //0x0010
	char pad_001C[12]; //0x001C
}; //Size: 0x0028

class BuildTable;
class BuildColumn_Components;
class BuildRow
{
public:
	BuildTable* buildTable; //0x0000
	char pad_0008[12]; //0x0008
	float Weight; //0x0014
	char pad_0018[8]; //0x0018
	BuildTag buildTag; //0x0020
	BuildTags buildTags; //0x0028
	char pad_0034[4]; //0x0034
	BuildColumn_Components* Components; //0x0038
	Vector4f* vectorValuesForColorSets; //0x0040
	char pad_0048[16]; //0x0048
}; //Size: 0x0058
assert_sizeof(BuildRow, 0x58);

class RowSelection
{
public:
	char pad_0[0x10];
};
assert_sizeof(RowSelection, 0x10);

class RowSelector
{
public:
	SharedPtrNew<BuildTable>* buildTableShared; //0x0000
	char pad_0008[22]; //0x0008
	SmallArray<RowSelection> arrRowSelection; //0x001E
	BuildTags buildTags; //0x002A
	char pad_0036[98]; //0x0036
}; //Size: 0x0098
assert_sizeof(RowSelector, 0x98);

class BuildColumn;
class BuildTable : public ManagedObject
{
public:
	SmallArray<BuildColumn> buildColumns; //0x0010
	SmallArray<BuildRow> buildRows; //0x0010
	char pad_0028[16]; //0x0028
	RowSelector rowSelector; //0x0038
	char pad_00D0[16]; //0x00D0
	SharedPtrNew<BuildTable>* selfSharedPtrE0; //0x00E0
}; //Size: 0x00E8
assert_sizeof(BuildTable, 0xE8);

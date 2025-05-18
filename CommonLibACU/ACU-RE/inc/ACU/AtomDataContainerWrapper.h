#pragma once

#include "basic_types.h"

enum class AtomDataContainerWrapper_DataType : uint32
{
	Bool = 0,
	Float = 1,
	Int = 2,
	XY = 4,
	XYZ = 5,
	XYZW = 6,

	NoData_mb = 10,
};

class AtomDataContainerWrapper
{
public:
	int32 value; //0x0000
	char pad_0004[12]; //0x0004
	AtomDataContainerWrapper_DataType DataType_0bool1float2int4xy5xyz6quat; //0x0010
	char pad_0014[12]; //0x0014
}; //Size: 0x0020
assert_sizeof(AtomDataContainerWrapper, 0x20);

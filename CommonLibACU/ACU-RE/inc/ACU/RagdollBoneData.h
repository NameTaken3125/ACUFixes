#pragma once

#include "basic_types.h"
#include "vmath/vmath.h"

enum class RagdollBodyPart : uint32
{
	RagdollReference = 0,
	RagdollHips = 1,
};
class RagdollBoneData
{
public:
	Matrix4f LocalShapeMatrix; //0x0000
	RagdollBodyPart BodyPart; //0x0040
	uint32 BoneID; //0x0044
	float Height; //0x0048
	float Radius; //0x004C
	float Mass; //0x0050
	float Friction; //0x0054
	float Restitution; //0x0058
	float LinearDamping; //0x005C
	float AngularDamping; //0x0060
	uint16 BodyPartMask; //0x0064
	char pad_0066[106]; //0x0066
}; //Size: 0x00D0
assert_sizeof(RagdollBoneData, 0xD0);

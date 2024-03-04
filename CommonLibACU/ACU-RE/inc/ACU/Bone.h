#pragma once

#include "vmath/vmath.h"
#include "ManagedObject.h"
#include "SmallArray.h"

class BoneModifier;

class Bone : public Object
{
public:
	char pad_0008[8]; //0x0008
	Vector4f GlobalPosition; //0x0010
	Vector4f GlobalRotation; //0x0020
	Vector4f LocalPosition; //0x0030
	Vector4f LocalRotation; //0x0040
	Bone* FatherBone; //0x0050
	Bone* MirrorBone; //0x0058
	uint32 MirroringType; //0x0060
	SmallArray<BoneModifier*> boneModifiers; //0x0064
	char pad_0070[12]; //0x0070
	uint32 WrinkleCategory; //0x007C
	uint32 BoneID_boneNameHash; //0x0080
	float WrinkleFactor; //0x0084
	uint16 Index; //0x0088
	uint16 ChildBoneCount; //0x008A
	char pad_008C[4]; //0x008C
}; //Size: 0x0090
assert_sizeof(Bone, 0x90);

#pragma once

#include "vmath/vmath.h"
#include "ManagedObject.h"
#include "SmallArray.h"

class BoneModifier;

class Bone : public Object
{
public:
	char pad_0008[8]; //0x0008
	Vector4f start; //0x0010
	Vector4f quat_mb_20; //0x0020
	Vector4f offsetToEnd; //0x0030
	Vector4f quat_mb_40; //0x0040
	Bone* bone_50; //0x0050
	Bone* bone_58; //0x0058
	char pad_0060[4]; //0x0060
	SmallArray<BoneModifier*> boneModifiers; //0x0064
	char pad_0070[32]; //0x0070
}; //Size: 0x0090
assert_sizeof(Bone, 0x90);

#pragma once

#include "vmath/vmath.h"
#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class Bone;
class BodyPartMapping;
class SkeletonPoseGroup;
class IKData;
class COMData;
class LookAtDataSkeletonDefinition;
class ReflexSystem;

class Skeleton : public ManagedObject
{
public:
	Vector4f addedOffsetToAllBones_mb; //0x0010
	Vector4f addedRotation_mb; //0x0020
	char pad_0030[4]; //0x0030
	SmallArray<Bone*> bones; //0x0034
	Bone* rootBone_mb; //0x0040
	SharedPtrNew<BodyPartMapping>* shared_BodyPartMapping; //0x0048
	SmallArray<SkeletonPoseGroup> arrPoseGroups; //0x0050
	char pad_005C[4]; //0x005C
	IKData* ikData; //0x0060
	COMData* COMData_; //0x0068
	SharedPtrNew<LookAtDataSkeletonDefinition>* shared_LookAtDataSkeletonDefinition; //0x0070
	ReflexSystem* reflexSystem; //0x0078
	char pad_0080[32]; //0x0080
}; //Size: 0x00A0
const int x = sizeof(Skeleton);
assert_sizeof(Skeleton, 0xA0);

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
	Vector4f RootBoneDefaultLocalPosition; //0x0010
	Vector4f RootBoneDefaultLocalRotation; //0x0020
	char pad_0030[4]; //0x0030
	SmallArray<Bone*> Bones; //0x0034
	Bone* RootBone; //0x0040
	SharedPtrNew<BodyPartMapping>* shared_BodyPartMapping; //0x0048
	SmallArray<SkeletonPoseGroup> PoseGroups; //0x0050
	char pad_005C[4]; //0x005C
	IKData* ikData; //0x0060
	COMData* COMData_; //0x0068
	SharedPtrNew<LookAtDataSkeletonDefinition>* shared_LookAtDataSkeletonDefinition; //0x0070
	ReflexSystem* reflexSystem; //0x0078
	uint32 SkeletonKey; //0x0080
	uint32 SkeletonHierarchyKey; //0x0084
	char pad_0088[1]; //0x0088
	SmallArray<void*> arr89; //0x0089
	char pad_0095[11]; //0x0095
}; //Size: 0x00A0
assert_sizeof(Skeleton, 0xA0);

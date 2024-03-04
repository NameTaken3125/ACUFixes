#pragma once

#include <array>

#include "Component.h"
#include "EventListener.h"
#include "SmallArray.h"
#include "SharedPtr.h"


class ExpressionSolver : public EventListener
{
public:
	char pad_0028[1848]; //0x0028
}; //Size: 0x0760
assert_sizeof(ExpressionSolver, 0x760);

#include "vmath/vmath.h"
class SkeletonComponent_470_1E8
{
public:
	Vector4f positionWorld; //0x0000
	Quaternion<float> quat_mb; //0x0010
}; //Size: 0x0020
assert_sizeof(SkeletonComponent_470_1E8, 0x20);

class SkeletonComponent;
class LookAtData;
class SkeletonComponent_470
{
public:
	char pad_0000[424]; //0x0000
	SkeletonComponent* skeletonCpnt; //0x01A8
	LookAtData* LookAtData_1B0; //0x01B0
	LookAtData* LookAtData_1B8; //0x01B8
	char pad_01C0[40]; //0x01C0
	SmallArray<SkeletonComponent_470_1E8> arrBodyPartBones_mb; //0x01E8
	char pad_01F4[84]; //0x01F4
}; //Size: 0x0248

class Skeleton;
class ExpressionPack;
class SkeletonComponent_5C;
class LookAtData;
class LookAtDataGeneral;
class MotionSmoother;
class BallJointChain;
class BoneModifier;
class IKData;
class SkeletonComponent_288;
class SkeletonComponent_470;
class AnimationCalculation_HasBoneLocalCalcs;
class SkeletonComponent_2A8;

struct BoneLocalCalc
{
	Vector3f pos;
	char pad_C[4];
	Quatf rot;
};
assert_sizeof(BoneLocalCalc, 0x20);
class AnimationCalculation_HasBoneLocalCalcs
{
public:
	char pad_0000[368]; //0x0000
	class SkeletonComponent_270* toBoneInfos; //0x0170
	std::array<BoneLocalCalc, 348>* boneLocalCalcs; //0x0178
	char pad_0180[16]; //0x0180
}; //Size: 0x0190
// Created with ReClass.NET 1.2 by KN4CK3R

class AnimationCalculation_BoneInfo;
class SkeletonComponent_270_68;
class SkeletonComponent_270_38
{
public:
	uint32 BoneID_mb; //0x0000
	uint16 boneCalcIdx; //0x0004
	char pad_0006[2]; //0x0006
}; //Size: 0x0008
assert_sizeof(SkeletonComponent_270_38, 8);

class SkeletonComponent_270
{
public:
	char pad_0000[8]; //0x0000
	SmallArray<AnimationCalculation_BoneInfo> boneInfos; //0x0008
	char pad_0014[22]; //0x0014
	uint16 word_2A; //0x002A
	char pad_002C[12]; //0x002C
	SmallArray<SkeletonComponent_270_38> boneCalcIndicesToUse_mb; //0x0038
	char pad_0044[12]; //0x0044
	void* arr50; //0x0050
	char pad_0058[2]; //0x0058
	uint16 arr50size; //0x005A
	char pad_005C[12]; //0x005C
	SmallArray<SkeletonComponent_270_68> arr68; //0x0068
	char pad_0074[76]; //0x0074
}; //Size: 0x00C0
assert_sizeof(SkeletonComponent_270, 0xC0);

class SkeletonComponent : public Component
{
public:
	char pad_0020[16]; //0x0020
	SharedPtrNew<Skeleton>* MainSkeleton; //0x0030
	SmallArray<SharedPtrNew<Skeleton>*> AddonSkeletons; //0x0038
	char pad_0044[4]; //0x0044
	SharedPtrNew<ExpressionPack>* shared_ExpressionPack; //0x0048
	char pad_0050[12]; //0x0050
	SmallArray<SkeletonComponent_5C> arr5C; //0x005C
	char pad_0068[16]; //0x0068
	SharedPtrNew<LookAtDataGeneral>* shared_LookAtDataGeneral; //0x0078
	LookAtData* lookAtData; //0x0080
	char pad_0088[24]; //0x0088
	MotionSmoother* motionSmoother; //0x00A0
	SharedPtrNew<SkeletonComponent>* selfSharedPtr; //0x00A8
	SmallArray<BallJointChain*> arrBallJointChains; //0x00B0
	char pad_00BC[44]; //0x00BC
	uint16 bits_3_6_areIdxOfBoneModsArray; //0x00E8
	char pad_00EA[342]; //0x00EA
	uint64 criticalSection_240; //0x0240
	char pad_0248[40]; //0x0248
	SkeletonComponent_270* hasBoneInfos; //0x0270
	char pad_0278[8]; //0x0278
	AnimationCalculation_HasBoneLocalCalcs* finalBlendedBoneRotationCalcs_mb; //0x0280		// Bone-local rotations and translations
	AnimationCalculation_HasBoneLocalCalcs* boneCalcsWithPosition_currentBuffer; //0x0288	// Entity-local rotations and translations
	AnimationCalculation_HasBoneLocalCalcs* boneCalcsWithPosition_backBuffer; //0x0290		// Entity-local rotations and translations
	AnimationCalculation_HasBoneLocalCalcs* boneCalcsWithPosition_bufferA; //0x0298			// Entity-local rotations and translations
	AnimationCalculation_HasBoneLocalCalcs* boneCalcsWithPosition_bufferB; //0x02A0			// Entity-local rotations and translations
	SkeletonComponent_2A8* toBoneLocalCalcs; //0x02A8
	char pad_02B0[32]; //0x02B0
	std::array< SmallArray<BoneModifier*>, 6> arraysOfBoneModifiers; //0x02D0
	char pad_0318[16]; //0x0318
	IKData* ikData; //0x0328
	char pad_0330[320]; //0x0330
	SkeletonComponent_470* p470; //0x0470
	char pad_0478[28]; //0x0478
	SmallArray<SkeletonComponent_5C> arr494; //0x0494
	char pad_04A0[48]; //0x04A0
	ExpressionSolver expressionSolver; //0x04D0
	char pad_0C30[120]; //0x0C30
	float float_CA8; //0x0CA8
	char pad_0CAC[18]; //0x0CAC
	uint16 word_CBE; //0x0CBE
	char pad_0CC0[16]; //0x0CC0
	uint8 byte_CD0; //0x0CD0
	uint8 byte_CD1; //0x0CD1
	char pad_0CD2[14]; //0x0CD2
}; //Size: 0x0CE0
assert_sizeof(SkeletonComponent, 0xCE0);
assert_offsetof(SkeletonComponent, p470, 0x470);

#pragma once

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

class Skeleton;
class ExpressionPack;
class SkeletonComponent_5C;
class LookAtData;
class LookAtDataGeneral;
class MotionSmoother;
class BallJointChain;
class BoneModifier;
class IKData;

class SkeletonComponent : public Component
{
public:
	char pad_0020[16]; //0x0020
	SharedPtrNew<Skeleton>* shared_BaseSkeletonMale; //0x0030
	SmallArray<SharedPtrNew<Skeleton>*> arrSharedSkeletons; //0x0038
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
	char pad_00BC[544]; //0x00BC
	SmallArray<BoneModifier*> boneModifiers_2DC; //0x02DC
	SmallArray<BoneModifier*> boneModifiers_2E8; //0x02E8
	SmallArray<BoneModifier*> boneModifiers_2F4; //0x02F4
	SmallArray<BoneModifier*> boneModifiers_300; //0x0300
	SmallArray<BoneModifier*> boneModifiers_30C; //0x030C
	char pad_0318[16]; //0x0318
	IKData* ikData; //0x0328
	char pad_0330[356]; //0x0330
	SmallArray<SkeletonComponent_5C> arr494; //0x0494
	char pad_04A0[48]; //0x04A0
	ExpressionSolver expressionSolver; //0x04D0
	char pad_0C30[176]; //0x0C30
}; //Size: 0x0CE0
assert_sizeof(SkeletonComponent, 0xCE0);

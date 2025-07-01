#pragma once

#include "basic_types.h"
#include "vmath/vmath.h"

class RagdollConstraintData
{
public:
	enum class Type
	{
		RagdollConstraint = 0,
		LimitedHingeConstraint = 1,
	};

	Matrix4f ChildBoneLocalMatrix; //0x0000
	Vector4f ChildSpaceAxisOrientation; //0x0040
	Vector4f TwistAxis; //0x0050
	Vector4f PlaneAxis; //0x0060
	Type ConstraintType; //0x0070
	uint32 ParentBoneID; //0x0074
	uint32 ChildBoneID; //0x0078
	float MaxFrictionTorque; //0x007C
	float AngularLimitTauFactor; //0x0080
	float MinAngularLimit; //0x0084
	float MaxAngularLimit; //0x0088
	float TwistMinAngularLimit; //0x008C
	float TwistMaxAngularLimit; //0x0090
	float ConeAngularLimit; //0x0094
	float ConeAxisRotation; //0x0098
	float PlaneMinAngularLimit; //0x009C
	float PlaneMaxAngularLimit; //0x00A0
	uint8 UseNewAxisSetup; //0x00A4
	char pad_00A5[11]; //0x00A5
}; //Size: 0x00D0
assert_sizeof(RagdollConstraintData, 0xB0);

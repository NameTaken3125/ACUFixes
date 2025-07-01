#pragma once

#include "vmath/vmath.h"
#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"

#include "RagdollBoneData.h"
#include "RagdollConstraintData.h"

enum class RagdollSkeletonType : uint32
{
	RagdollSkeletonType_Biped = 0,
};

class RagdollMotorParameters
{
public:
	float Tau; //0x0000
	float Damping; //0x0004
	float MaxForce; //0x0008
	float ProportionalRecoveryVelocity; //0x000C
	float ConstantRecoveryVelocity; //0x0010
	char pad_0014[12]; //0x0014
}; //Size: 0x0020
assert_sizeof(RagdollMotorParameters, 0x20);

class ImpactData;
class CollisionMaterial;

class RagdollNew : public ManagedObject
{
public:
	Vector4f ReferenceToRemappedDefaultTranslation; //0x0010
	Vector4f ReferenceToRemappedDefaultRotation; //0x0020
	RagdollSkeletonType RagdollSkeletonType_; //0x0030
	SmallArray<RagdollBoneData> BoneData; //0x0034
	SmallArray<RagdollConstraintData> ConstraintData; //0x0040
	char pad_004C[12]; //0x004C
	RagdollBodyPart ReferenceBoneRemapTarget; //0x0058
	char pad_005C[4]; //0x005C
	SharedPtrNew<ImpactData>* ImpactData_; //0x0060
	SharedPtrNew<CollisionMaterial>* CollisionMaterialTemplate; //0x0068
	SmallArray<uint32> BoneHierarchyOrder; //0x0070
	float Stiffness; //0x007C
	float LinearDamping; //0x0080
	float AngularDamping; //0x0084
	float PinocchioPositionStiffness; //0x0088
	float PinocchioVelocityStiffness; //0x008C
	float PinocchioAccelerationStiffness; //0x0090
	float PinocchioAngleStiffness; //0x0094
	float PinocchioAngularVelocityStiffness; //0x0098
	float PinocchioAngularAccelerationStiffness; //0x009C
	RagdollMotorParameters RagdollConstraintTwistMotor; //0x00A0
	RagdollMotorParameters RagdollConstraintPlaneMotor; //0x00C0
	RagdollMotorParameters RagdollConstraintConeMotor; //0x00E0
	RagdollMotorParameters LimitedHingeContraintMotor; //0x0100
	char pad_0120[48]; //0x0120
}; //Size: 0x0150
assert_sizeof(RagdollNew, 0x150);

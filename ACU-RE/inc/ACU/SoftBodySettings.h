#pragma once

#include "vmath/vmath.h"
#include "ACU/basic_types.h"
#include "ACU/ManagedObject.h"

#pragma pack(push, 1)
class Serialized_SoftBodySettings
{
public:
    char pad_0000[2]; //0x0000
    uint64 handle; //0x0002
    uint32 hashInTypeInfosSystem; //0x000A
    float WindRandomFactor; //0x000E
    bool EnablePhysics; //0x0012
    bool UseWind; //0x0013
    bool UseGravityBone; //0x0014
    uint8 NormalsUpdateFrequency; //0x0015
    uint32 GravityBoneID; //0x0016
    float GravityBoneScale; //0x001A
    float SkinningLinearVelocityFactor; //0x001E
    float SkinningAngularVelocityFactor; //0x0022
    float SkinningMaxFadeoutSpeed; //0x0026
    float StructuralStiffness; //0x002A
    float StructuralConstraintMinCorrectionFactor; //0x002E
    float StructuralConstraintMaxCorrectionFactor; //0x0032
    float ShearingStiffness; //0x0036
    float ShearingConstraintMinCorrectionFactor; //0x003A
    float ShearingConstraintMaxCorrectionFactor; //0x003E
    float BendingStiffness; //0x0042
    float BendingConstraintMinCorrectionFactor; //0x0046
    float BendingConstraintMaxCorrectionFactor; //0x004A
    bool StretchConstraintEnabled; //0x004E
    float StretchConstraintFactor; //0x004F
    bool MaxDistanceEnabled; //0x0053
    float ScaleMaxDistance; //0x0054
    bool BackStopEnabled; //0x0058
    float ScaleBackstop; //0x0059
    bool PerVertexGravityEnabled; //0x005D
    bool PerVertexDampingEnabled; //0x005E
    bool PerVertexSkinWidthEnabled; //0x005F
    bool PerVertexFrictionEnabled; //0x0060
    bool CollisionAgainstPrimitive; //0x0061
    bool CollisionAgainstCloth; //0x0062
    uint8 PriorityForClothClothCollision; //0x0063
    bool ContinuousCollisionDetection; //0x0064
    float Friction; //0x0065
    float ClothSkinWidth; //0x0069
    bool UseAdditionalVertices; //0x006D
    float ResolutionForAdditionalVertices; //0x006E
    float Damping; //0x0072
    uint8 NumIterations; //0x0076
    Vector3f ClothGravity; //0x0077
    float EnvironmentDensity; //0x0083
    float ClothMaterialDensity; //0x0087
    bool Structural; //0x008B
    bool Shearing; //0x008C
    bool Bending; //0x008D
}; //Size: 0x008E
#pragma pack(pop)
assert_offsetof(Serialized_SoftBodySettings, WindRandomFactor, 0xE);
assert_offsetof(Serialized_SoftBodySettings, UseWind, 0x13);
assert_offsetof(Serialized_SoftBodySettings, ClothGravity, 0x77);
assert_sizeof(Serialized_SoftBodySettings, 0x8E);

struct SoftBodySettings_Data
{
    float WindRandomFactor; //0x0010
    float ClothSkinWidth; //0x0014
    float ResolutionForAdditionalVertices; //0x0018
    float Damping; //0x001C
    uint32 GravityBoneID; //0x0020
    float GravityBoneScale; //0x0024
    float SkinningLinearVelocityFactor; //0x0028
    float SkinningAngularVelocityFactor; //0x002C
    float SkinningMaxFadeoutSpeed; //0x0030
    float StructuralStiffness; //0x0034
    float StructuralConstraintMinCorrectionFactor; //0x0038
    float StructuralConstraintMaxCorrectionFactor; //0x003C
    float ShearingStiffness; //0x0040
    float ShearingConstraintMinCorrectionFactor; //0x0044
    float ShearingConstraintMaxCorrectionFactor; //0x0048
    float BendingStiffness; //0x004C
    float BendingConstraintMinCorrectionFactor; //0x0050
    float BendingConstraintMaxCorrectionFactor; //0x0054
    float EnvironmentDensity; //0x0058
    float StretchConstraintFactor; //0x005C
    float ClothMaterialDensity; //0x0060
    float ScaleMaxDistance; //0x0064
    Vector3f ClothGravity; //0x0068
    float ScaleBackstop; //0x0074
    float Friction; //0x0078
    bool EnablePhysics : 1;
    bool UseWind : 1;
    bool UseGravityBone : 1;
    uint8 NormalsUpdateFrequency; //0x007D
    bool StretchConstraintEnabled; //0x007E
    bool MaxDistanceEnabled; //0x007F
    uint8 PriorityForClothClothCollision; //0x0080
    bool ContinuousCollisionDetection; //0x0081
    bool PerVertexGravityEnabled; //0x0082
    bool PerVertexDampingEnabled; //0x0083
    bool UseAdditionalVertices; //0x0084
    bool PerVertexSkinWidthEnabled; //0x0085
    bool PerVertexFrictionEnabled; //0x0086
    uint8 NumIterations; //0x0087
    bool BackStopEnabled; //0x0088
    bool CollisionAgainstPrimitive; //0x0089
    bool CollisionAgainstCloth; //0x008A
    bool Structural; //0x008B
    bool Shearing; //0x008C
    bool Bending; //0x008D
    char pad_008E[2]; //0x008E
};
assert_sizeof(SoftBodySettings_Data, 0x80);


class SoftBodySettings : public ManagedObject
{
public:
    SoftBodySettings_Data m_data; //0x0010
}; //Size: 0x0090
assert_offsetof(SoftBodySettings, m_data.ClothGravity, 0x68);
assert_offsetof(SoftBodySettings, m_data.NormalsUpdateFrequency, 0x7D);
assert_sizeof(SoftBodySettings, 0x90);

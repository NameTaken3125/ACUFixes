#pragma once

#include "BaseEntity.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class Component;

class Entity_CpntIndices_157
{
public:
    char pad_0000[1]; //0x0000
    uint8 atomAnimCpnt; //0x0001
    char pad_0002[6]; //0x0002
}; //Size: 0x0008

class Entity_CpntIndices_15F
{
public:
    uint8 byte_15f; //0x0000
    uint8 cpntIdx_BhvAssassin_or_AIComponent; //0x0001
    char pad_0002[3]; //0x0002
}; //Size: 0x0005
assert_sizeof(Entity_CpntIndices_15F, 5);


class EntityDescriptor
{
public:
    uint32 DescriptorType : 3;
    uint32 SubDescriptorType : 6;
    uint32 ExplicitProperty : 6;
    uint32 IsPickableByPlayer : 1;
    uint32 IsPickableByNPC : 1;

    uint32 bits_0x11_0x13 : 3;

    uint32 ADBEntryID : 6;
    uint32 prop_8CE8468D : 1;
};
const int x = sizeof(EntityDescriptor);
assert_sizeof(EntityDescriptor, 4);
using bool_64 = uint64;
struct EntityFlags
{
    bool_64 SkipGroupMatrixUpdate : 1;
    bool_64 IsGraphicsUnitTestCameraReferencePosition : 1;
    uint64 bits_2_5 : (6 - 2);
    bool_64 IsPhantom : 1;
    bool_64 IsHidden : 1;
    bool_64 IsSmallObject : 1;
    bool_64 IsMediumObject : 1;
    uint64 RenderLast : 2;
    bool_64 ForceIncludedInReflection : 1;
    bool_64 ForceExcludedFromReflection : 1;
    bool_64 UsePlanarDistanceForGraphicObject : 1;
    uint64 bits_15_23 : (24 - 15);
    bool_64 IsCharacter : 1;
    bool_64 ShadowCaster : 1;
    bool_64 ShadowReceiver : 1;
    bool_64 SunlightReceiver : 1;
    bool_64 DynamicLightsReceiver : 1;
    uint64 bits_29_30 : (31 - 29);
    bool_64 UseVisibilityQueries : 1;
    bool_64 VisibilityOccluder : 1;
    bool_64 AlwaysVisibilityOccluder : 1;
    bool_64 IgnoreOcclusionForLODLevel : 1;

    uint64 bits_35_42 : (43 - 35);

    uint64 EntityCategory : 5;
    bool_64 UserMask1 : 1;
    bool_64 UserMask2 : 1;
    bool_64 UserMask3 : 1;
    bool_64 UserMask4 : 1;
    bool_64 UserMask5 : 1;

    uint64 bits_53_57 : (58 - 53);

    bool_64 UIFlag : 1;

    uint64 bits_59_61 : (62 - 59);

    bool_64 prop_B0EF08A7 : 1;
    bool_64 NeverDynamicInStencil : 1;
};
assert_sizeof(EntityFlags, 8);

class BoundingVolume
{
public:
	uint32 Type; //0x0000
	Vector3f Min; //0x0004
	Vector3f Max; //0x0010
	char pad_001C[12]; //0x001C
}; //Size: 0x0028
assert_sizeof(BoundingVolume, 0x28);
class DataLayerAction;
class DataLayerFilter
{
public:
    SmallArray<DataLayerAction> LayerActions; //0x0000
}; //Size: 0x000C
assert_sizeof(DataLayerFilter, 0xC);

class GameStateData;

class Entity : public BaseEntity
{
public:

    // @members
    SmallArray<Component*> cpnts_mb; //0x0078
    char pad_0084[4]; //0x0084
    EntityFlags flags88; //0x0088 // Read below.
    GameStateData* ResetData; //0x0090
    BoundingVolume BoundingVolume_; //0x0098
    char pad_00C0[8]; //0x00C0
    DataLayerFilter DataLayerFilter_; //0x00C8
    EntityDescriptor EntityDescriptor_; //0x00D4
    char pad_00D8[88]; //0x00D8
    SharedPtrNew<Entity>* selfSharedPtr; //0x0130
    char pad_0138[31]; //0x0138
    Entity_CpntIndices_157 cpntIndices_157; //0x0157
    Entity_CpntIndices_15F cpntIndices; //0x015F
    char pad_0164[12]; //0x0164

    // @helper_functions
    Component* FindComponentByVTBL(uint64 vtbl);
    static TypeInfo& GetTI() { return *(TypeInfo*)0x144E6ABB0; }
}; //Size: 0x0170
assert_offsetof(Entity, selfSharedPtr, 0x130);
assert_offsetof(Entity, cpntIndices_157.atomAnimCpnt, 0x158);
assert_sizeof(Entity, 0x0170);

/*
* If `(Entity::flags88 >> 58) & 1 == 1`, then animations will not be affected by bendtime.
* However, actual movement speed will be, and Quickshot projectile might not fly after animation finishes.
*/

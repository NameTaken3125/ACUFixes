#pragma once

#include "vmath/vmath.h"
#include "Component.h"
#include "SmallArray.h"

enum class GuidanceSystemSubType : uint32
{
    GuidanceObjectSubTypeNone = 0,
    GuidanceObjectSubTypeLedgeGrab = 1,
    GuidanceObjectSubTypeBeam = 2,
    GuidanceObjectSubTypeLadder = 3,
    GuidanceObjectSubTypePole = 4,
    GuidanceObjectSubTypeRope = 5,
    GuidanceObjectSubTypeSurface = 6,
    GuidanceObjectSubTypeQuadruped = 7,
    GuidanceObjectSubTypeKiosk = 8,
    GuidanceObjectSubTypeBoatPlank = 9,
    GuidanceObjectSubTypeBoatInnerEndEdge = 10,
    GuidanceObjectSubTypeBoatInnerSideEdge = 11,
    GuidanceObjectSubTypeBoatMiddleEdge = 12,
    GuidanceObjectSubTypeHayStack = 13,
    GuidanceObjectSubTypeLedgeGrabHandsOnly = 14,
    GuidanceObjectSubTypeFork_Hangfork = 15,
    GuidanceObjectSubTypeFork_Hangwall = 16,
    GuidanceObjectSubTypeParallel = 17,
    GuidanceObjectSubTypeHorizontalParallel = 18,
    GuidanceObjectSubTypeLedgeGrabPole = 19,
    GuidanceObjectSubTypePassOver = 20,
    GuidanceObjectSubTypeLedgeBeam = 21,
    GuidanceObjectSubTypeNoHandGuidance = 22,
    GuidanceObjectSubTypeClimbLadder = 23,
    GuidanceObjectSubTypeWindow = 24,
    GuidanceObjectSubTypeDoor = 25,
};

class GuidanceObject
{
public:
    uint32 Valid : 1;
    GuidanceSystemSubType SubType : 5;
    uint32 Index0 : 13;
    uint32 Index1 : 13;
    uint32 DecN4Normal0 : 30;
    uint32 bits_4_0x1E_0x20 : 2;
    uint32 DecN4Normal1 : 30;
    uint32 bits_8_0x1E_0x20 : 2;
};
assert_sizeof(GuidanceObject, 0xC);

class EdgeFilter
{
public:
    float SlopeCosAngle; //0x0000
    float WallAngle; //0x0004
    float MinZDiffAngle; //0x0008
    float SlopeCosAngleForWallAgainstGroundOrCeiling; //0x000C

    bool EdgeWallWall : 1;
    bool EdgeWallGround : 1;
    bool EdgeWallCeiling : 1;
    bool EdgeWallVoid : 1;
    bool EdgeGroundCeiling : 1;
    bool EdgeGroundVoid : 1;
    bool EdgeCeilingVoid : 1;
    bool UseSlopeAngleForWallAgainstGroundOrCeiling : 1;

    char pad_0011[3]; //0x0011
}; //Size: 0x0014
assert_sizeof(EdgeFilter, 0x14);

class GuidanceObject;

class GuidanceSystemComponent : public Component
{
public:
    GuidanceObject* GuidanceObjects; //0x0020
    char pad_0028[8]; //0x0028
    SmallArray<Vector3f> Points; //0x0030
    EdgeFilter EdgeFilter_; //0x003C
    uint16 FirstParallelIndex; //0x0050
    uint8 Dynamic; //0x0052
    char pad_0053[5]; //0x0053
}; //Size: 0x0058
assert_sizeof(GuidanceSystemComponent, 0x58);

class Partitioner;

class GuidanceSystem : public GuidanceSystemComponent
{
public:
    Partitioner* Partitioner_; //0x0058
    uint8 IsOmniDirectionalGuidance; //0x0060
    char pad_0061[7]; //0x0061
}; //Size: 0x0068
assert_sizeof(GuidanceSystem, 0x68);

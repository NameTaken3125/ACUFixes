#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class AsymmetricDamping : public Object
{
public:
    char pad_0008[8]; //0x0008
}; //Size: 0x0010

class BlendCurveData : public Object
{
public:
    char pad_0008[16]; //0x0008
}; //Size: 0x0018

class MainBehaviorTargetTracker;
class HorizontalCurvePoint;
class CameraData : public Object
{
public:
    char pad_0008[96]; //0x0008
    AsymmetricDamping asymmetricDamping_68; //0x0068
    AsymmetricDamping ad_78; //0x0078
    char pad_0088[64]; //0x0088
    BlendCurveData blendCurveData_c8; //0x00C8
    char pad_00E0[56]; //0x00E0
    MainBehaviorTargetTracker* mainBehaviorTargetTracker_118; //0x0118
    char pad_0120[464]; //0x0120
    SmallArray<HorizontalCurvePoint*> horizontalCurvePts; //0x02F0
    char pad_02FC[532]; //0x02FC
}; //Size: 0x0510
assert_sizeof(CameraData, 0x510);

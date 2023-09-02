#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class VerticalCurvePoint;
class HorizontalCurvePoint : public Object
{
public:
    SmallArray<VerticalCurvePoint*> verticalCurvePts; //0x0008
    char pad_0014[4]; //0x0010
}; //Size: 0x0018
assert_sizeof(HorizontalCurvePoint, 0x18);

class PointData : public Object
{
public:
    char pad_0008[72]; //0x0008
    float distanceFromPlayer; //0x0050
    float flt_54; //0x0054
    float flt_58; //0x0058
    float fov; //0x005C
    char pad_0060[64]; //0x0060
}; //Size: 0x00A0
assert_sizeof(PointData, 0xA0);

class VerticalCurvePoint : public Object
{
public:
    char pad_0008[8]; //0x0008
    PointData pointData; //0x0010
    char pad_00B0[16]; //0x00B0
}; //Size: 0x00C0
assert_sizeof(VerticalCurvePoint, 0xC0);

#pragma once

#include "vmath/vmath.h"
#include "ManagedObject.h"

class ITargetPrecision : public Object
{
public:
    char pad_0008[32]; //0x0008
}; //Size: 0x0028
assert_sizeof(ITargetPrecision, 0x28);

class ThrowTargetPrecision : public ITargetPrecision
{
public:
    char pad_0028[8]; //0x0028
    Vector4f trackerCrawlsTowardPredictorBeamEnd; //0x0030
    char pad_0040[36]; //0x0040
    float angleZ; //0x0064
    float distanceToBeamEnd; //0x0068
    float heightWhereTheCameraAims; //0x006C
    Vector4f predictionBeamOrigin; //0x0070
    float flt_80; //0x0080
    float flt_84; //0x0084
    float flt_88; //0x0088
    float flt_8C; //0x008C
    float flt_90; //0x0090
    char pad_0094[12]; //0x0094
}; //Size: 0x00A0
assert_sizeof(ThrowTargetPrecision, 0xA0);

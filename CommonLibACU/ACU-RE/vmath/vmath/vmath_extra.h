#pragma once

#ifdef VMATH_NAMESPACE
namespace VMATH_NAMESPACE
{
#endif //VMATH_NAMESPACE
#include "vmath.h"
struct ChooseLessAlignedVector
{
    Vector3f moreAligned;
    Vector3f lessAligned;
    ChooseLessAlignedVector(const Vector3f& alignedWith, const Vector3f& _1, const Vector3f _2)
    {
        float al1 = abs(alignedWith.dotProduct(_1));
        float al2 = abs(alignedWith.dotProduct(_2));
        moreAligned = (al1 < al2) ? _2 : _1;
        lessAligned = (al1 < al2) ? _1 : _2;
    }
};
inline Matrix3f MakeRotationAlignZWithVector(Vector3f axisZ)
{
    axisZ.normalize();
    const Vector3f regularX = { 1, 0, 0 };
    const Vector3f regularY = { 0, 1, 0 };
    ChooseLessAlignedVector otherAxes{ axisZ, regularX, regularY };
    Vector3f axisX = otherAxes.lessAligned;
    Vector3f axisY = axisX.crossProduct(axisZ).normalized();
    axisX.normalize();
    float result[9] = {
        axisX.x, axisX.y, axisX.z,
        axisY.x, axisY.y, axisY.z,
        axisZ.x, axisZ.y, axisZ.z };
    return result;
}

#ifdef VMATH_NAMESPACE
}
#endif //VMATH_NAMESPACE
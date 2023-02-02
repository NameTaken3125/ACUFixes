#pragma once

#include "vmath/vmath.h"
class PotentialWindowEntry
{
public:
    char pad_0000[8]; //0x0000
    void* vtbl8; //0x0008
    Vector4f positionHandsFrom; //0x0010
    Vector4f normalFromWall_20; //0x0020
    Vector4f windowLedgeCenterPosition; //0x0030
    Vector4f normalFromWall_40; //0x0040
    Vector4f windowLedgeRightPoint; //0x0050
    Vector4f windowLedgeLeftPoint; //0x0060
    Vector4f normalToWall_70; //0x0070
    Vector4f normalToWall_80; //0x0080
    Vector4f vec90_expectedMovementTowardEntrance_mb; //0x0090
    Vector4f vecA0_towardEntranceHorizontal_mb; //0x00A0
    char pad_00B0[28]; //0x00B0
    float dotProductWithMovementVec_mb; //0x00CC
    float flt_d0; //0x00D0
    char pad_00D4[172]; //0x00D4
    Vector4f vec180; //0x0180
    Vector4f vec190; //0x0190
    Vector4f vec1A0; //0x01A0
    char pad_01B0[56]; //0x01B0
    float flt_1e8_dotProductRequired_mb; //0x01E8
    float flt_1ec; //0x01EC
    char pad_01F0[16]; //0x01F0
    float flt_200_epsilon_mb; //0x0200
    char pad_0204[268]; //0x0204
}; //Size: 0x0310

#pragma once

#include "vmath/vmath.h"

class RenderValuesHolder
{
public:
    // @members
    Matrix4f cameraTransform; //0x0000
    char pad_0040[540]; //0x0040
    float fov; //0x025C
    char pad_0260[40]; //0x0260
    float viewportWidth_pixels; //0x0288
    float viewportHeight_pixels; //0x028C
    char pad_0290[12]; //0x0290
    Vector4f vec_29c; //0x029C
    char pad_02AC[16]; //0x02AC
    Vector4f vec_2bc_hasViewportSize; //0x02BC
    float aspectRatio; //0x02CC
    char pad_02D0[80]; //0x02D0
    Matrix4f matProjection_mb; //0x0320
    char pad_0360[336]; //0x0360

    // @helper_functions
    static RenderValuesHolder* GetSingleton() { return *(RenderValuesHolder**)0x14521AAA8; }
}; //Size: 0x04B0

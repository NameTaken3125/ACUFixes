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
    Vector2f viewportSizeBeforeStretching; //0x0288 // For example, if the resolution is set to 640*400, windowed, then the size of window is 640*400, but this value is still 640*360 no matter if letterbox is present or the Stretching is enabled.
    char pad_0290[12]; //0x0290
    Vector4f vec_29c; //0x029C
    char pad_02AC[16]; //0x02AC
    Vector4f vec_2bc_hasViewportSize; //0x02BC
    float aspectRatio; //0x02CC
    char pad_02D0[16]; //0x02D0
    Matrix4f matViewProjection; //0x02E0
    Matrix4f matProjection; //0x0320
    Matrix4f mat_360; //0x0360
    Matrix4f matInvProjection; //0x03A0
    char pad_03E0[320]; //0x03E0

    // @helper_functions
    static RenderValuesHolder* GetSingleton() { return *(RenderValuesHolder**)0x14521AAA8; }
}; //Size: 0x04B0
assert_offsetof(RenderValuesHolder, matInvProjection, 0x3A0);

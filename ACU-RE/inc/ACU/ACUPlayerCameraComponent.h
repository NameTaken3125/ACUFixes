#pragma once

#include "vmath/vmath.h"

#include "Component.h"

class PlayerCameraComponent : public Component
{
public:
    char pad_0020[48]; //0x0020
    Vector4f position; //0x0050
    Vector4f quaternion_mb; //0x0060
    float fov_mb_pi_4; //0x0070
    char pad_0074[172]; //0x0074
}; //Size: 0x0120
assert_sizeof(PlayerCameraComponent, 0x120);

class CameraData;
class ObjectRegistry_Entry;
class ACUPlayerCameraComponent_9C0;

class ACUPlayerCameraComponent : public PlayerCameraComponent
{
public:
    // @members
    char pad_0120[1936]; //0x0120
    Vector4f cameraTopRight_mb; //0x08B0
    char pad_08C0[16]; //0x08C0
    float fovPrecalc; //0x08D0
    char pad_08D4[36]; //0x08D4
    CameraData* cameraData; //0x08F8
    char pad_0900[76]; //0x0900
    float flt_94c; //0x094C
    char pad_0950[4]; //0x0950
    float flt_954; //0x0954
    char pad_0958[96]; //0x0958
    ObjectRegistry_Entry* currentCameraSelectorBlenderNode; //0x09B8
    ACUPlayerCameraComponent_9C0* cameraCurrentBlending_mb_9c0; //0x09C0
    char pad_09C8[200]; //0x09C8
    Vector4f belowCameraCenter_mb; //0x0A90
    char pad_0AA0[96]; //0x0AA0
    __int64 qword_b00; //0x0B00
    char pad_0B08[1992]; //0x0B08

    // @helper_functions
    static ACUPlayerCameraComponent* GetSingleton();
}; //Size: 0x12D0
assert_sizeof(ACUPlayerCameraComponent, 0x12D0);

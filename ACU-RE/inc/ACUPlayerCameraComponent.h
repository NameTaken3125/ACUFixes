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

class ACUPlayerCameraComponent : public PlayerCameraComponent
{
public:
    // @members
    char pad_0120[4528]; //0x0120

    // @helper_functions
    static ACUPlayerCameraComponent* GetSingleton();
}; //Size: 0x12D0
assert_sizeof(ACUPlayerCameraComponent, 0x12D0);

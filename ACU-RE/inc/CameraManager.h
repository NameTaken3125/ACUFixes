#pragma once

#include "MandatoryUniverseComponent.h"

class ACUPlayerCameraComponent;


class CameraManager : public MandatoryUniverseComponent
{
public:
    // @members
    char pad_0010[48]; //0x0010
    ACUPlayerCameraComponent** arr_to_ACUPlayerCameraComponent; //0x0040
    char pad_0048[80]; //0x0048

    // helper_functions
    static CameraManager* GetSingleton() { return *(CameraManager**)0x14521AAD0; }
}; //Size: 0x0098
assert_sizeof(CameraManager, 0x98);

#pragma once

#include "MandatoryUniverseComponent.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class ACUPlayerCameraComponent;
class CameraSelectorNode;
class FixedCameraSelectorNode;
class HideSpotTargetTracker;

class CameraManager : public MandatoryUniverseComponent
{
public:
    // @members
    SharedPtrNew<CameraSelectorNode>* shared_CameraSelectorNode_ACU_Camera_Root; //0x0010
    SharedPtrNew<CameraSelectorNode>* shared_CameraSelectorNode_CinematicTransitionProfiles; //0x0018
    SharedPtrNew<FixedCameraSelectorNode>* shared_FixedCameraSelectorNode_FixedCameras; //0x0020
    SmallArray<HideSpotTargetTracker*> hideSpotTargetTrackers; //0x0028
    char pad_0034[4]; //0x0034
    class CameraTransitionInfo* cameraTransitionInfo; //0x0038
    SmallArray<ACUPlayerCameraComponent*> arr_to_ACUPlayerCameraComponent; //0x0040
    char pad_004C[76]; //0x004C

    // @helper_functions
    static CameraManager* GetSingleton() { return *(CameraManager**)0x14521AAD0; }
}; //Size: 0x0098
assert_sizeof(CameraManager, 0x98);

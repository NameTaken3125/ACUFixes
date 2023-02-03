#include "pch.h"

#include "ACUPlayerCameraComponent.h"
#include "CameraManager.h"
#include "SmallArray.h"

ACUPlayerCameraComponent* ACUPlayerCameraComponent::GetSingleton()
{
    CameraManager* camMgr = CameraManager::GetSingleton();
    if (!camMgr) { return nullptr; }
    auto& camCpntArray = camMgr->arr_to_ACUPlayerCameraComponent;
    if (camCpntArray.size == 0) { return nullptr; }
    return camCpntArray.arr[0];
}

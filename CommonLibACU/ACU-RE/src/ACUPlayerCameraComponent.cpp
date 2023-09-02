#include "pch.h"

#include "ACU/ACUPlayerCameraComponent.h"
#include "ACU/CameraManager.h"
#include "ACU/SmallArray.h"

ACUPlayerCameraComponent* ACUPlayerCameraComponent::GetSingleton()
{
    CameraManager* camMgr = CameraManager::GetSingleton();
    if (!camMgr) { return nullptr; }
    auto& camCpntArray = camMgr->arr_to_ACUPlayerCameraComponent;
    if (camCpntArray.size == 0) { return nullptr; }
    return camCpntArray.arr[0];
}

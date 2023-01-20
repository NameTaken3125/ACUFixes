#include "pch.h"

#include "ACUPlayerCameraComponent.h"
#include "CameraManager.h"

ACUPlayerCameraComponent* ACUPlayerCameraComponent::GetSingleton()
{
    CameraManager* camMgr = CameraManager::GetSingleton();
    if (!camMgr) { return nullptr; }
    ACUPlayerCameraComponent** camCpntArray = camMgr->arr_to_ACUPlayerCameraComponent;
    if (!camCpntArray) { return nullptr; }
    return *camCpntArray;
}

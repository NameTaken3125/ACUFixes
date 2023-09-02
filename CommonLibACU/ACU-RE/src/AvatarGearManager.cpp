#include "pch.h"

#include "ACU/AvatarGearManager.h"
#include "ACU/GamePlaySettings.h"

AvatarGearManager* AvatarGearManager::GetSingleton()
{
    auto* gps = GamePlaySettings::GetSingleton();
    if (!gps) { return nullptr; }
    AvatarGearManager* agm = gps->sharedPtr_AvatarGearManager->GetPtr();
    return agm;
}

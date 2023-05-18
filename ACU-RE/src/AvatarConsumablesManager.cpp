#include "pch.h"

#include "ACU/AvatarConsumablesManager.h"
#include "ACU/GamePlaySettings.h"

AvatarConsumablesManager* AvatarConsumablesManager::GetSingleton()
{
	auto* gps = GamePlaySettings::GetSingleton();
	if (!gps) { return nullptr; }
	return gps->shared_AvatarConsumablesManager->GetPtr();
}

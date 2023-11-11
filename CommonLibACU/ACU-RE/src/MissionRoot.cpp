#include "pch.h"

#include "ACU/MissionManager.h"
#include "ACU/MissionRoot.h"

MissionRoot* MissionRoot::GetSingleton()
{
    MissionManager* misMan = MissionManager::GetSingleton();
    if (!misMan) { return nullptr; }
    return misMan->shared_MissionRoot->GetPtr();
}

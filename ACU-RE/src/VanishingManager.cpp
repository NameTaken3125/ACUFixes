#include "pch.h"

#include "ACU/VanishingManager.h"
#include "ACU/World.h"

VanishingManager* VanishingManager::GetSingleton()
{
    World* world = World::GetSingleton();
    if (!world) { return nullptr; }
    return world->vanishingManager;
}

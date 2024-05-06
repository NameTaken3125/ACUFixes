#include "pch.h"

#include "ACU/World.h"
#include "ACU/WorldTagRules.h"

WorldTagRules* WorldTagRules::GetSingleton()
{
    World* world = World::GetSingleton();
    if (!world) { return nullptr; }
    return world->worldTagRules;
}
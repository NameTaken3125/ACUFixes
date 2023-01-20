#include "pch.h"

#include "BhvAssassin.h"
#include "World.h"

BhvAssassin* BhvAssassin::GetSingletonPlayer()
{
    World* world = World::GetSingleton();
    if (!world) { return nullptr; }
    BhvAssassin** arr = world->array_toBhvAssassin_218;
    if (!arr) { return nullptr; }
    return *arr;
}

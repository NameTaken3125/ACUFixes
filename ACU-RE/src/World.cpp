#include "pch.h"

#include "World.h"
#include "Entity.h"
#include "ACUPlayerCameraComponent.h"


World* World::GetSingleton()
{
    ACUPlayerCameraComponent* camCpnt = ACUPlayerCameraComponent::GetSingleton();
    if (!camCpnt) { return nullptr; }
    Entity* player = camCpnt->entity;
    if (!player) { return nullptr; }
    return player->world;
}

#include "pch.h"

#include "ACU/World.h"
#include "ACU/Entity.h"
#include "ACU/ACUPlayerCameraComponent.h"


World* World::GetSingleton()
{
    ACUPlayerCameraComponent* camCpnt = ACUPlayerCameraComponent::GetSingleton();
    if (!camCpnt) { return nullptr; }
    Entity* player = camCpnt->entity;
    if (!player) { return nullptr; }
    return player->world;
}

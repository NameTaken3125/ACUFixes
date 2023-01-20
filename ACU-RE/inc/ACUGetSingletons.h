#pragma once

class World;
class Entity;
class BhvAssassin;
class ACUPlayerCameraComponent;
class CSrvPlayerHealth;

namespace ACU {

World* GetWorld();
Entity* GetPlayer();
CSrvPlayerHealth* GetPlayerHealth();
BhvAssassin* GetPlayerBhvAssassin();
ACUPlayerCameraComponent* GetPlayerCameraComponent();

}
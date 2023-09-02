#pragma once

#include "basic_types.h"

class World;
class Entity;
class BhvAssassin;
class ACUPlayerCameraComponent;
class CSrvPlayerHealth;

struct IDXGISwapchain;

namespace ACU {

World* GetWorld();
Entity* GetPlayer();
CSrvPlayerHealth* GetPlayerHealth();
BhvAssassin* GetPlayerBhvAssassin();
ACUPlayerCameraComponent* GetPlayerCameraComponent();

uint64 GetWindowHandle();
IDXGISwapchain* GetSwapchain();

}
#pragma once

#include "basic_types.h"

class World;
class Entity;
class BhvAssassin;
class ACUPlayerCameraComponent;
class SkeletonComponent;
class AtomAnimComponent;
class CSrvPlayerHealth;

struct IDXGISwapchain;

namespace ACU {

World* GetWorld();
Entity* GetPlayer();
CSrvPlayerHealth* GetPlayerHealth();
BhvAssassin* GetPlayerBhvAssassin();
ACUPlayerCameraComponent* GetPlayerCameraComponent();
SkeletonComponent* GetPlayerCpnt_SkeletonComponent();
AtomAnimComponent* GetPlayerCpnt_AtomAnimComponent();

uint64 GetWindowHandle();
IDXGISwapchain* GetSwapchain();

} // namespace ACU

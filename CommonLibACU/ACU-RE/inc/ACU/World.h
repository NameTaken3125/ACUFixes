#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "Clock.h"

class BhvAssassin;

class BaseWorld : public ManagedObject
{
public:
    char pad_0010[192]; //0x0010
}; //Size: 0x00D0
assert_sizeof(BaseWorld, 0xD0);

class WorldComponent;
class UIWorldComponent;
class ConflictLoopManager;
class DeferredCastManager;
class VanishingManager;
class TimeOfDayManager;
class GameplayCoordinatorWorldComponent;
class RiftWorldManager;
class WorldTagRules;
class Entity;
class World : public BaseWorld
{
public:
    // @members
    char pad_00D0[8]; //0x00D0
    Entity* player_d8; //0x00D8
    char pad_00E0[8]; //0x00E0
    void* contactManager; //0x00E8
    char pad_00F0[16]; //0x00F0
    void* worldGraphicData; //0x0100
    void* fxManager; //0x0108
    char pad_0110[8]; //0x0110
    SmallArray<WorldComponent*> worldComponents; //0x0118
    char pad_0124[244]; //0x0120
    BhvAssassin** array_toBhvAssassin_218; //0x0218
    char pad_0220[1464]; //0x0220
    Clock clockInWorldWithSlowmotion; //0x07D8 // Slowmotion effect is created by overriding the timescale here.
    Clock clockGameFocusedWithoutSlowmotion; //0x0830 // Still ticks if game is paused but doesn't tick while Alt+Tabbed out of game window.
    Clock clockUnpausedWithoutSlowmotion; //0x0888 // Unaffected by slowmotion, but stops ticking while paused.
    char pad_08E0[16]; //0x08E0
    ConflictLoopManager* conflictLoopManager; //0x08F0
    char pad_08F8[24]; //0x08F8
    VanishingManager* vanishingManager; //0x0910
    char pad_0918[88]; //0x0918
    DeferredCastManager* deferredCastManager; //0x0970
    char pad_0978[8]; //0x0978
    TimeOfDayManager* timeOfDayManager; //0x0980
    GameplayCoordinatorWorldComponent* gameplayCoordinatorWorldComponent; //0x0988
    char pad_0990[56]; //0x0990
    UIWorldComponent* uiWorldComponent; //0x09C8
    char pad_09D0[24]; //0x09D0
    RiftWorldManager* riftWorldManager; //0x09E8
    char pad_09F0[16]; //0x09F0
    WorldTagRules* worldTagRules; //0x0A00
    char pad_0A08[96]; //0x0A08

    // @helper_functions
    static World* GetSingleton();
}; //Size: 0x0A68
assert_offsetof(World, worldComponents, 0x0118);
assert_offsetof(World, array_toBhvAssassin_218, 0x218);
assert_offsetof(World, clockInWorldWithSlowmotion, 0x7D8);
assert_offsetof(World, conflictLoopManager, 0x8F0);
assert_offsetof(World, uiWorldComponent, 0x09C8);
assert_sizeof(World, 0x0A68);

#include "pch.h"

#include "ACU/ACUGetSingletons.h"
#include "ACU/ACUPlayerCameraComponent.h"
#include "ACU/Entity.h"
#include "ACU/BhvAssassin.h"
#include "ACU/World.h"
#include "ACU/CharacterAI.h"
#include "ACU/Statechart.h"

#include "ACU/HasSwapchain.h"

namespace ACU {

    ACUPlayerCameraComponent* GetPlayerCameraComponent() { return ACUPlayerCameraComponent::GetSingleton(); }
    Entity* GetPlayer()
    {
        auto camCpnt = GetPlayerCameraComponent(); if (!camCpnt) { return nullptr; }
        return camCpnt->entity;
    }
    World* GetWorld() { return World::GetSingleton(); }
    BhvAssassin* GetPlayerBhvAssassin() { return BhvAssassin::GetSingletonPlayer(); }
    CSrvPlayerHealth* GetPlayerHealth()
    {
        BhvAssassin* bhvAssassin = GetPlayerBhvAssassin(); if (!bhvAssassin) { return nullptr; }
        CharacterAI* charAI = bhvAssassin->characterAI_toHealthAndInventory; if (!charAI) { return nullptr; }
        struct PlayerCharacterAI_ChildStatecharts
        {
            char pad_0000[64]; //0x0000
            class CSrvNavigation* csrvNavigation; //0x0040
            char pad_0048[72]; //0x0048
            class CSrvPlayerHealth* health; //0x0090
            char pad_0098[200]; //0x0098
            class Statechart* humanFightInteractions; //0x0160
            char pad_0168[72]; //0x0168
        }; //Size: 0x01B0
        auto& childStatecharts = charAI->childStatecharts_mb_toHealth_60; if (!childStatecharts.arr) { return nullptr; }
        CSrvPlayerHealth* health = ((PlayerCharacterAI_ChildStatecharts*)childStatecharts.arr)->health;
        return health;
    }
    uint64 GetWindowHandle()
    {
        /*
        Also:
        145136FD0, 14523AA38, 14525BED8
        [HasSwapchain+6f0]
        [[HasSwapchain+6f8]+8]
        [[HasSwapchain+6f8]+70]
        */
        return *(uint64*)0x145136FD0;
    }
    IDXGISwapchain* GetSwapchain()
    {
        HasSwapchain* hsc = HasSwapchain::GetSingleton();
        if (!hsc) return nullptr;
        if (!hsc->toSwapchain) return nullptr;
        return hsc->toSwapchain->swapchain;
    }

}
#include "pch.h"

#include "Cheat_Ammo.h"

#include "ACU/HumanStatesHolder.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU/WeaponComponent.h"
#include "ACU/CharacterAI.h"
#include "ACU/Entity.h"
#include "ACU/ManagedPtrs/ManagedPtrs.h"
#include "ACU_DefineNativeFunction.h"

struct Human1C8
{
    uint64 vtbl;
};
DEFINE_GAME_FUNCTION(Entity__Get_Human1C8, 0x140C17A50, Human1C8*, __fastcall, (Entity* entity));
DEFINE_GAME_FUNCTION(Human_1C8__DecreaseFirearmAmmo, 0x140C05780, void, __fastcall, (Human1C8* p_human1C8));
DEFINE_GAME_FUNCTION(Human1C8__GetCurrentRangedWeaponShared_mb, 0x140C15130, ACU::StrongRef<Entity>*, __fastcall, (Human1C8* p_human1C8, ACU::StrongRef<Entity>&, char a3));
DEFINE_GAME_FUNCTION(Entity__Get_WeaponComponent, 0x140C1B7E0, WeaponComponent*, __fastcall, (Entity* p_entity));
void WhenDecreasingTheRemainingShotsUntilReload_SkipIfPlayer(AllRegisters* params)
{
    auto* humanStates = (HumanStatesHolder*)params->rcx_;
    Entity* entity = humanStates->ownerEntity;
    auto human1C8 = Entity__Get_Human1C8(entity);
    Entity* player = ACU::GetPlayer();
    const bool isPlayer = player && player == entity;
    if (human1C8 && !isPlayer)
        Human_1C8__DecreaseFirearmAmmo(human1C8);
}
void Entity__DecreaseRemainingAmmoUntilReloadQuickshotInCombat__FullReplacement(Entity* entity, int decreaseAmount)
{
    Human1C8* human1C8 = Entity__Get_Human1C8(entity);
    if (!human1C8) return;
    ACU::StrongRef<Entity> weaponEntityRef;
    ACU::StrongRef<Entity>* foundWpn = Human1C8__GetCurrentRangedWeaponShared_mb(human1C8, weaponEntityRef, 0);
    Entity* weaponEnt = foundWpn->GetPtr();
    if (!weaponEnt) return;
    WeaponComponent* weaponCpnt = Entity__Get_WeaponComponent(weaponEnt);
    if (!weaponCpnt) return;
    auto* remainingAmmo = weaponCpnt->remainingAmmo;
    if (!remainingAmmo) return;
    int oldAmount = remainingAmmo->ammoTillReload;
    int newAmount = 0;
    if (decreaseAmount < oldAmount)
        newAmount = oldAmount - decreaseAmount;
    remainingAmmo->ammoTillReload = newAmount;
}
struct InventoryDataItem_8
{
    virtual void Unk000();
    virtual void Unk008();
    virtual void Unk010();
    virtual CharacterAI* Unk018_GetAI();
    virtual void Unk020();
    virtual void Unk028();
    virtual void Unk030();
};
void WhenDecreasingRemainingAmmoBeforeInventoryUpdateEvent_SkipForPlayer(AllRegisters* params)
{
    auto* idi = (InventoryDataItem_8*)params->rsi_;
    CharacterAI* ai = idi->Unk018_GetAI();
    if (!ai) return;
    Entity* entity = ai->entity;
    if (!entity) return;
    const bool isPlayer = entity == ACU::GetPlayer();
    int& decreaseAmount = (int&)params->rdi_;
    int& newAmount = (int&)params->rcx_;
    if (isPlayer)
        newAmount += decreaseAmount;
}
AmmoCheat::AmmoCheat()
{
    uintptr_t whenDecreasingTheRemainingShotsUntilReload = 0x140C05814;
    PresetScript_NOP(whenDecreasingTheRemainingShotsUntilReload, 2);
    uintptr_t whenDecreasingTheRemainingShotsUntilReload_quickshotInCombat = 0x1409E7227;
    PresetScript_NOP(whenDecreasingTheRemainingShotsUntilReload_quickshotInCombat, 3);
    uintptr_t whenDecreasingRemainingAmmo_rangedWeaponsAfterReload = 0x140FE053E;
    PresetScript_NOP(whenDecreasingRemainingAmmo_rangedWeaponsAfterReload, 3);
    PresetScript_CCodeInTheMiddle(0x140FDA3BB, 6,
        WhenDecreasingRemainingAmmoBeforeInventoryUpdateEvent_SkipForPlayer, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    //uintptr_t whenDecreasingRemainingAmmo = 0x140FDA3C4;
    //PresetScript_NOP(whenDecreasingRemainingAmmo, 3);
    //PresetScript_CCodeInTheMiddle(0x141A0D96B, 7,
    //    WhenDecreasingTheRemainingShotsUntilReload_SkipIfPlayer, 0x141A0D984, false);
    //PresetScript_ReplaceFunctionAtItsStart(0x1409E71A0, Entity__DecreaseRemainingAmmoUntilReloadQuickshotInCombat__FullReplacement);
}

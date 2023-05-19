#include "pch.h"
#include "Hack_ReloadRangedWeaponsWhenRefillAllInShop.h"

#include "ACU/WeaponComponent.h"
#include "ACU/Entity.h"
#include "ACU/Human.h"

WeaponComponent* GetWeaponComponent(Entity& entity)
{
    constexpr uint64 WeaponComponent__VTable = 0x142F01050;
    return (WeaponComponent*)(entity.FindComponentByVTBL(WeaponComponent__VTable));
}
void ReloadAllRangedWeapons()
{
    Human* human = Human::GetForPlayer();
    if (!human) { return; }
    for (auto& boneMaybe : human->arr358)
    {
        for (auto& maybeHasWeapon : boneMaybe.arrSmthWithSkeletonComponentsAndEntities)
        {
            Entity* potentialWeaponEntity = maybeHasWeapon.shared_entity->GetPtr();
            if (!potentialWeaponEntity) { continue; }
            WeaponComponent* weaponCpnt = GetWeaponComponent(*potentialWeaponEntity);
            if (!weaponCpnt) { continue; }
            if (!weaponCpnt->remainingAmmo) { continue; }
            uint32 maxAmmoInMagazine = weaponCpnt->GetMaxAmmoInMagazine();
            weaponCpnt->remainingAmmo->ammoTillReload = maxAmmoInMagazine;
        }
    }
}

void WhenRefillAllInShop_AlsoReloadWeapons(AllRegisters* params)
{
    ReloadAllRangedWeapons();
}
ReloadRangedWeaponsWhenRefillAllInShop::ReloadRangedWeaponsWhenRefillAllInShop()
{
    uintptr_t whenRefillAllInShop = 0x140D37B50;
    PresetScript_CCodeInTheMiddle(whenRefillAllInShop, 5,
        WhenRefillAllInShop_AlsoReloadWeapons, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

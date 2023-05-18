#include "pch.h"

#include "ACU/WeaponComponent.h"

uint32 WeaponComponent::GetMaxAmmoInMagazine()
{
    auto* nfw = netFightWeapon->GetPtr();
    if (!nfw) { return 0; }
    if (!nfw->magazineData) { return 0; }
    return nfw->magazineData->maxAmmoInMagazine;
}

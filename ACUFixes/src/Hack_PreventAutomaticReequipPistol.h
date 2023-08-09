#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"


struct PreventAutomaticReequipPistol : AutoAssemblerCodeHolder_Base
{
    PreventAutomaticReequipPistol();
};
PreventAutomaticReequipPistol::PreventAutomaticReequipPistol()
{
    uintptr_t whenBestPistolAboutToBeSelectedAfterSwitchingFromTwohandedRangedWeaponToNonrangedMelee = 0x140D2B613;
    PresetScript_NOP(whenBestPistolAboutToBeSelectedAfterSwitchingFromTwohandedRangedWeaponToNonrangedMelee, 5);
}

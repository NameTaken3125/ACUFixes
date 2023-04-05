#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct AmmoCheat : AutoAssemblerCodeHolder_Base
{
    AmmoCheat();
};
AmmoCheat::AmmoCheat()
{
    uintptr_t whenDecreasingTheRemainingShotsUntilReload = 0x140C05814;
    PresetScript_NOP(whenDecreasingTheRemainingShotsUntilReload, 2);
    uintptr_t whenDecreasingTheRemainingShotsUntilReload_quickshotInCombat = 0x1409E7227;
    PresetScript_NOP(whenDecreasingTheRemainingShotsUntilReload_quickshotInCombat, 3);
    uintptr_t whenDecreasingRemainingAmmo = 0x140FDA3C4;
    PresetScript_NOP(whenDecreasingRemainingAmmo, 3);
}

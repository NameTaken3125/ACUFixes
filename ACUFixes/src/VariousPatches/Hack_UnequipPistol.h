#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

// No longer used since I added the "Unequip pistol" button.
struct PreventAutomaticReequipPistol : AutoAssemblerCodeHolder_Base
{
    PreventAutomaticReequipPistol();
};

struct UnequipPistol : AutoAssemblerCodeHolder_Base
{
    UnequipPistol();
};

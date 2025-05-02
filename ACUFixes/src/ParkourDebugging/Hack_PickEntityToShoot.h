#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct PickEntityToShoot : public AutoAssemblerCodeHolder_Base
{
	PickEntityToShoot();
};
void RaycastPicker_PickRangedWeaponTarget();

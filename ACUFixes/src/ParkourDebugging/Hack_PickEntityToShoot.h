#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct PickEntityToShoot : public AutoAssemblerCodeHolder_Base
{
	PickEntityToShoot();
	virtual void OnBeforeActivate() override;
	virtual void OnBeforeDeactivate() override;
};
void RaycastPicker_PickRangedWeaponTarget();

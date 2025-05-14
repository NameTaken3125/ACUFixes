#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct ParkourDebuggingPatch : public AutoAssemblerCodeHolder_Base
{
	ParkourDebuggingPatch();
	virtual void OnBeforeActivate() override;
	virtual void OnBeforeDeactivate() override;
};

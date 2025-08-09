#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"
#include "EnumParkourAction.h"
#include "GenericHooksInParkourFiltering.h"

struct ParkourDebuggingPatch : public AutoAssemblerCodeHolder_Base
{
	ParkourDebuggingPatch();
	virtual void OnBeforeActivate() override;
	virtual void OnBeforeDeactivate() override;

	std::shared_ptr<GenericHooksInParkourFiltering> m_GPH;
	std::shared_ptr<SharedHookActivator> m_Activator_GPHCreation;
	std::shared_ptr<SharedHookActivator> m_Activator_GPHSortAndSelect;
};

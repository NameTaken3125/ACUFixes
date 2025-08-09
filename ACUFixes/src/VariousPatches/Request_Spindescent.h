#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"
#include "ParkourDebugging/GenericHooksInParkourFiltering.h"

class ParkourCallbacksForParkourHelpers
{
public:
    AvailableParkourAction* ChooseBeforeFiltering(SmallArray<AvailableParkourAction*>& actions);
    AvailableParkourAction* ChooseAfterSorting(SmallArray<AvailableParkourAction*>& actions, AvailableParkourAction* selectedByGame);

    AvailableParkourAction* m_SelectedSpindescent = nullptr;

    ParkourCallbacks m_Callbacks;
    ParkourCallbacksForParkourHelpers();
};
struct ParkourActionsExtraProcessing : AutoAssemblerCodeHolder_Base
{
    ParkourActionsExtraProcessing();
    virtual void OnBeforeActivate() override;
    virtual void OnBeforeDeactivate() override;

    std::shared_ptr<SharedHookActivator> m_Activator_GPHSortAndSelect;
    std::unique_ptr<ParkourCallbacksForParkourHelpers> m_ParkourCallbacksForParkourHelpers;

private:
    std::shared_ptr<GenericHooksInParkourFiltering> m_GPH;
};

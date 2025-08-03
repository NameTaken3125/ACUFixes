#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"
#include "ParkourDebugging/GenericHooksInParkourFiltering.h"

class ParkourCallbacksForParkourHelpers : public ParkourCallbacks
{
public:
    virtual AvailableParkourAction* ChooseBeforeFiltering(SmallArray<AvailableParkourAction*>& actions) override;
    virtual AvailableParkourAction* ChooseAfterSorting(SmallArray<AvailableParkourAction*>& actions, AvailableParkourAction* selectedByGame) override;

    AvailableParkourAction* m_SelectedSpindescent = nullptr;
};
struct ParkourActionsExtraProcessing : AutoAssemblerCodeHolder_Base
{
    ParkourActionsExtraProcessing();
    virtual void OnBeforeActivate() override;
    virtual void OnBeforeDeactivate() override;

    std::shared_ptr<SharedHookActivator> m_Activator_GPHSortAndSelect;
    std::unique_ptr<ParkourCallbacksForParkourHelpers> m_ParkourCallbacksForParkourHelpers;
};

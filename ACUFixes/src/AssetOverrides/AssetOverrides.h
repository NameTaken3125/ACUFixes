#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct AddVirtualForges : public AutoAssemblerCodeHolder_Base
{
    AddVirtualForges();
    virtual void OnBeforeActivate() override;
    virtual void OnBeforeDeactivate() override;
};

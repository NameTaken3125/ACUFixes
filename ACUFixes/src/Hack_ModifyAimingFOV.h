#pragma once

#include "AutoAssemblerKinda.h"

class ModifyConditionalFOVs : public AutoAssemblerCodeHolder_Base
{
public:
    ModifyConditionalFOVs();
    virtual void OnBeforeDeactivate() override;
};

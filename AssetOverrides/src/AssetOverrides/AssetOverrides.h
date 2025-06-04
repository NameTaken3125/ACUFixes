#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct AddVirtualForges : public AutoAssemblerCodeHolder_Base
{
    AddVirtualForges();
    virtual void OnBeforeDeactivate() override;
};

void AssetOverrides_InitFromLoadOrder();
void AssetOverrides_EarlyHooks_Start();
void AssetOverrides_EarlyHooks_End();
void AssetOverrides_CodePatches_Start();

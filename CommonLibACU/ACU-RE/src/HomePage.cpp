#include "pch.h"

#include "ACU/HomePage.h"
#include "ACU/MenuManager.h"


constexpr uint64 vtbl_HomePage = 0x142EE9570;
HomePage* HomePage::GetSingleton()
{
    auto* menuMan = MenuManager::GetSingleton(); if (!menuMan) return nullptr;
    if (!menuMan->menuPagesStack_mb) return nullptr;
    if (menuMan->menuPagesStack_mb->arrHasMenuPages.size <= 0) return nullptr;
    for (auto* pageInMenuStack : menuMan->menuPagesStack_mb->arrHasMenuPages)
    {
        MenuPage* page = pageInMenuStack->shared_MenuPage->GetPtr();
        if (!page) continue;
        uint64 vtbl = *(uint64*)page;
        if (vtbl != vtbl_HomePage) continue;
        return static_cast<HomePage*>(page);
    }
    return nullptr;
}

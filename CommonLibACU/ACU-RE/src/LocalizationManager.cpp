#include "pch.h"

#include "ACU/LocalizationManager.h"
#include "ACU_DefineNativeFunction.h"

DEFINE_GAME_FUNCTION(CStringBuffer__DeallocateBuffer_mb, 0x14026BD70, ACU_WStringBuffer*, __fastcall, (ACU_WStringBuffer* this_, char a2));
DEFINE_GAME_FUNCTION(LocalizationManager__GetString_mb, 0x141DF44C0, __int64, __fastcall, (LocalizationManager* p_locManager, UIString p_stringID, ACU_WStringBuffer* p_stringOut));
void ACU_WStringBuffer::Clear()
{
    CStringBuffer__DeallocateBuffer_mb(this, 0);
}
ACU_WStringBuffer::ACU_WStringBuffer(UIString uiString)
{
    LocalizationManager* locMan = LocalizationManager::GetSingleton();
    if (!locMan) { return; }
    LocalizationManager__GetString_mb(locMan, uiString, this);
}
ACU_WStringBuffer::~ACU_WStringBuffer()
{
    Clear();
}

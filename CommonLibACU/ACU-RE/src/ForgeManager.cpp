#include "pch.h"

#include "ACU/ForgeManager.h"
#include "ACU_DefineNativeFunction.h"

DEFINE_GAME_FUNCTION(ForgeFile__ctor, 0x142718900, ForgeFile*, __fastcall, (ForgeFile* a1));

ForgeFile::ForgeFile()
{
    ForgeFile__ctor(this);
}

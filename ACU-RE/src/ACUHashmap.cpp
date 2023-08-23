#include "pch.h"
#include "ACU/ACUHashmap.h"

#include "../../ACUFixes/src/ACU_DefineNativeFunction.h"

DEFINE_GAME_FUNCTION(FindInACUHashmap, 0x14277AA00, void*, __fastcall, (void* p_hashmap, unsigned __int64 p_key, __int64 p_errorOut_mb));

void* ACUHashmapGeneric::Get(uint64 key)
{
    return FindInACUHashmap(dataPtrWith3LowestBitsAbused, key, 0);
}

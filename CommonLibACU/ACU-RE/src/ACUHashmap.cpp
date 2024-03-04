#include "pch.h"
#include "ACU/ACUHashmap.h"

#include "ACU_DefineNativeFunction.h"

DEFINE_GAME_FUNCTION(HashmapGeneric__Get, 0x14277AA00, void*, __fastcall, (void* p_hashmap, unsigned __int64 p_key, __int64 p_errorOut_mb));

void* ACUHashmapGeneric::Get(uint64 key)
{
    return HashmapGeneric__Get(dataPtrWith3LowestBitsAbused, key, 0);
}

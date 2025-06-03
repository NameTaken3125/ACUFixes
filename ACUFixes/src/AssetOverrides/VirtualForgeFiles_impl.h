#pragma once

#include "ACU/ForgeManager.h"
#include "ACU/Memory/ACUAllocs.h"

#include "MyLog.h"
static DEFINE_LOGGER_CONSOLE_AND_FILE(VirtualForgesLog, "[AssetOverrides]");

// Pay no attention to this for now.
enum class LoadPriority
{
    Lowest = 0,
    Highest = 1,
};
using ForgeIndex_t = uint32;
ForgeFileEntry* MakeNewForgeFileEntry(uint64 targetHandle, const fs::path& absoluteFilepath, LoadPriority priority);
bool IsForgeAlive(ForgeManager& fm, ForgeIndex_t forgeIdx);
DEFINE_GAME_FUNCTION(ForgeManager__DecrementForgeEntryRefcount_mb, 0x142721FE0, void, __fastcall, (ForgeManager* a1, int p_forgeIdx));

#pragma once

#include "ACU/ForgeManager.h"
#include "ACU/Memory/ACUAllocs.h"

#include "MyLog.h"
static DEFINE_LOGGER_CONSOLE_AND_FILE(VirtualForgesLog, "[AssetOverrides]");

// Inserts a new ForgeFileEntry* into the ForgeManager->forges
// at position 0 (topmost priority when looking up datapacks by handle).
ForgeFileEntry* MakeNewForgeFileEntry(uint64 targetHandle, const fs::path& absoluteFilepath);

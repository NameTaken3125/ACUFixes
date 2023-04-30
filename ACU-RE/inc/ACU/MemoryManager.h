#pragma once

#include <array>
#include "basic_types.h"

// Thanks `translu` for the "Assassin's Creed Unity gear unlocker"
// https://fearlessrevolution.com/viewtopic.php?t=6587
class MemoryManager
{
public:
    using Flags_64kbBlock = byte;
    using Descriptor_4gbBlock = std::array<Flags_64kbBlock, 0x10000>;
    using Descriptors_4gbBlocks = std::array<Descriptor_4gbBlock*, 0x10000>;

    // @members
    char pad_0000[880]; //0x0000
    Descriptors_4gbBlocks* refextra; //0x0370
    char pad_0378[200]; //0x0378

    // @helper_functions
    static MemoryManager& GetSingleton() { return **(MemoryManager**)(0x145221E08); }
}; //Size: 0x0440

#pragma once

#include "basic_types.h"

class ACUHashmapData;
class ACUHashmapGeneric
{
public:
    // @members
    void* dataPtrWith3LowestBitsAbused;

    // @helper_functions
public:
    // Returns ptr to element, or `nullptr` if not found.
    void* Get(uint64 key);
    ACUHashmapData& GetActualDataAddress()
    {
        return *(ACUHashmapData*)(*(uint64*)this & 0xFFFFFFFFFFFFFFF8);
    }
    uint8 GetFlagsInLowestBits()
    {
        return (uint64)this & 7;
    }
};
assert_sizeof(ACUHashmapGeneric, 8);

template<typename Key_t, typename Value_t>
class ACUHashmap
{
public:
    ACUHashmapGeneric m_map;

    // Returns `nullptr` if not found.
    Value_t* Get(Key_t key)
    {
        return (Value_t*)m_map.Get((uint64)key);
    }
};

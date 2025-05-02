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
    ACUHashmapGeneric() : dataPtrWith3LowestBitsAbused(nullptr) {}
    uint64 AddElement(uint64 key) {
        // Returns ptr to element, or `-1` if fails for some reason, I guess?
        // But actual usages in game code don't check for this failure.
        const auto HashmapGeneric__AddElement = (int64(__fastcall*)(ACUHashmapGeneric* p_hashmap, unsigned __int64 p_key, __int64 p_errorOut_mb))0x14277B2A0;
        return HashmapGeneric__AddElement(this, key, 0);
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
    void Set(Key_t key, Value_t value) { *(Value_t*)m_map.AddElement(key) = value; }
};

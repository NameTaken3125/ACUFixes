#pragma once

#include "basic_types.h"

template<typename T>
class BigArray
{
public:
    T* arr;
    uint32 size;
    uint32 capacityAndFlags;

    using iterator = T*;
    iterator begin() { return arr; }
    iterator end() { return arr + size; }
    auto rbegin() { return std::reverse_iterator(arr + size); }
    auto rend() { return std::reverse_iterator(arr); }

    uint32 GetCapacity() { return capacityAndFlags & 0x1FFFFFFF; }

    T& operator[](int idx) { return arr[idx]; }

    BigArray()
        : arr(nullptr)
        , size(0)
        , capacityAndFlags(0xC0000000)
    {}
};

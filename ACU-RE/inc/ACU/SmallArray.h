#pragma once

#include "basic_types.h"

// You can see that the size == 0xC in objects with multiple of these in a row.
#pragma pack(push, 1)
template<typename T>
class SmallArray
{
public:
    T* arr;
    uint16 capacityAndFlags;
    uint16 size;

    using iterator = T*;
    iterator begin() { return arr; }
    iterator end() { return arr + size; }
    auto rbegin() { return std::reverse_iterator(arr + size); }
    auto rend() { return std::reverse_iterator(arr); }

    T& operator[](int idx) { return arr[idx]; }
}; // Size: 0xC
#pragma pack(pop)

namespace {
    using TEST_SmallArray = SmallArray<float>;
    assert_sizeof(TEST_SmallArray, 0xC);
    assert_offsetof(TEST_SmallArray, capacityAndFlags, 8);
    assert_offsetof(TEST_SmallArray, size, 0xA);
}
template<typename T, size_t NumStaticElements>
class SmallArraySemistatic : public SmallArray<T>
{
public:
    T staticElements[NumStaticElements];
};

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

    SmallArray() : arr(nullptr), capacityAndFlags(0), size(0) {}
    SmallArray(const SmallArray& rhs) = delete;
    SmallArray& operator=(const SmallArray& rhs) = delete;
    SmallArray(SmallArray&& rhs) noexcept;
    SmallArray& operator=(SmallArray&& rhs) noexcept;

    void erase(iterator fromWhere, iterator toWhere);
}; // Size: 0xC
#pragma pack(pop)

template<typename T>
SmallArray<T>::SmallArray(SmallArray<T>&& rhs) noexcept
    : arr(rhs.arr)
    , capacityAndFlags(rhs.capacityAndFlags)
    , size(rhs.size)
{
    rhs.arr = nullptr;
    rhs.capacityAndFlags = 0;
    rhs.size = 0;
}

template<typename T>
SmallArray<T>& SmallArray<T>::operator=(SmallArray<T>&& rhs) noexcept
{
    arr = rhs.arr;
    capacityAndFlags = rhs.capacityAndFlags;
    size = rhs.size;
    rhs.arr = nullptr;
    rhs.capacityAndFlags = 0;
    rhs.size = 0;
    return *this;
}

template<typename T>
void SmallArray<T>::erase(iterator fromWhere, iterator toWhere)
{
    while (fromWhere != toWhere)
    {
        fromWhere->~T();
        fromWhere++;
        this->size--;
    }
}

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
    SmallArraySemistatic()
    {
        this->capacityAndFlags = 0x8000 | NumStaticElements;
        this->arr = reinterpret_cast<T*>(staticElements);
    }
    std::aligned_storage_t<sizeof(T), alignof(T)> staticElements[NumStaticElements];
};

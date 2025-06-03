#pragma once

/*
The "API" is a mess here, because I'm not sure how many heaps the game uses
and how they interact with threads. These functions have been useful,
but they definitely need attention. Wouldn't be surprised if they cause problems
when used in new contexts.
*/

#include <string_view>

#include "ACU/basic_types.h"
#include "ACU/TypeInfo.h"
#include "ACU/SmallArray.h"
#include "ACU_DefineNativeFunction.h"

namespace ACU::Memory
{

byte* ACUAllocateBytes(uint32 numBytes, uint32 alignment);
void ACUDeallocateBytes(byte* allocated);
inline char* ACUAllocateString(const std::string_view& s)
{
    char* cstring = reinterpret_cast<char*>(ACUAllocateBytes((uint32)s.size() + 1, 1));
    std::memcpy(cstring, s.data(), s.size());
    cstring[s.size()] = 0;
    return cstring;
}

DEFINE_GAME_FUNCTION(SmallArray_generic__AllocateSeparateBuffer, 0x14271F780, void*, __fastcall, (void* p_smallArray, int p_newCap, unsigned int p_align_mb, int p_elementSize, int a5, void* p_heap_mb));
DEFINE_GAME_FUNCTION(SmallArray_generic__SetNewBuffer, 0x1427260D0, void, __fastcall, (void* p_smallArray, void* p_newBuffer, int p_numElems, __int16 p_capacity, int p_align, char a6, char a7, char a8, char a9));


template<typename GameTypeWithTypeInfo>
void ACUConstruct(GameTypeWithTypeInfo& obj)
{
    std::memset(&obj, 0, sizeof(GameTypeWithTypeInfo));
    TypeInfo& ti = GameTypeWithTypeInfo::GetTI();
    ti.Create(&obj);
}
template<typename GameTypeWithTypeInfo>
GameTypeWithTypeInfo* ACUAllocate()
{
    byte* allocatedBytes = ACUAllocateBytes(sizeof(GameTypeWithTypeInfo), alignof(GameTypeWithTypeInfo));
    ACUConstruct(*(GameTypeWithTypeInfo*)allocatedBytes);
    return (GameTypeWithTypeInfo*)allocatedBytes;
}
template<typename GameTypeWithTypeInfo>
void SmallArray_GameType_Reserve(SmallArray<GameTypeWithTypeInfo>& arrInOut, uint16 newCapacity, void* heap_mb = nullptr)
{
    TypeInfo& ti = GameTypeWithTypeInfo::GetTI();

    // Implementation from `SmallArray_DWORD::reserve()` at 000000014201EDC0.

    if (!heap_mb)
    {
        void* g_currentHeap_mb = *(void**)0x1452585D0;
        heap_mb = g_currentHeap_mb;
    }

    uint16 curCapacity = arrInOut.capacityAndFlags & 0x3FFF;
    GameTypeWithTypeInfo* newBuffer = (GameTypeWithTypeInfo*)SmallArray_generic__AllocateSeparateBuffer(&arrInOut, newCapacity, alignof(GameTypeWithTypeInfo), sizeof(GameTypeWithTypeInfo), 0, heap_mb);
    GameTypeWithTypeInfo* oldBuffer = arrInOut.arr;
    for (size_t i = 0; i < arrInOut.size; i++)
    {
        newBuffer[i] = std::move(oldBuffer[i]);
    }
    SmallArray_generic__SetNewBuffer(&arrInOut, newBuffer, arrInOut.size, newCapacity, alignof(GameTypeWithTypeInfo), 1, 1, 1, 1);
}
template<typename ElementType>
void SmallArrayReserve(SmallArray<ElementType>& arrInOut, uint16 newCapacity, void* heap_mb = nullptr)
{
    // Implementation from `SmallArray_DWORD::reserve()` at 000000014201EDC0.

    if (!heap_mb)
    {
        void* g_currentHeap_mb = *(void**)0x1452585D0;
        heap_mb = g_currentHeap_mb;
    }

    uint16 curCapacity = arrInOut.capacityAndFlags & 0x3FFF;
    ElementType* newBuffer = (ElementType*)SmallArray_generic__AllocateSeparateBuffer(&arrInOut, newCapacity, alignof(ElementType), sizeof(ElementType), 0, heap_mb);
    ElementType* oldBuffer = arrInOut.arr;
    for (size_t i = 0; i < arrInOut.size; i++)
    {
        newBuffer[i] = oldBuffer[i];
    }
    SmallArray_generic__SetNewBuffer(&arrInOut, newBuffer, arrInOut.size, newCapacity, alignof(ElementType), 1, 1, 1, 1);
}
template<typename GameTypeWithTypeInfo>
GameTypeWithTypeInfo* SmallArray_GameType_Append(SmallArray<GameTypeWithTypeInfo>& arrInOut)
{
    // Implementation from `SmallArray_DWORD::AppendIfNotFound()` at 0000000140B8E8F0.

    uint16 newCap = 0x3FFF;
    uint16 oldCap = arrInOut.capacityAndFlags & 0x3FFF;
    if (arrInOut.size + 1 > oldCap)
    {
        if (oldCap == 0x3FFF)
        {
            // Max capacity, cannot increase. Crash on purpose.
            *(int*)0 = 0;
        }
        if (oldCap >= 0x18)
            newCap = oldCap / 3u + oldCap;
        else
            newCap = oldCap + 4;
        void* g_currentHeap_mb = *(void**)0x1452585D0;
        SmallArray_GameType_Reserve(arrInOut, newCap, g_currentHeap_mb);
    }
    uint16 newSize = arrInOut.size + 1;
    ACUConstruct(arrInOut.arr[newSize - 1]);
    arrInOut.size = newSize;
    return &arrInOut.arr[newSize - 1];
}
template<typename GameTypeWithTypeInfo>
GameTypeWithTypeInfo* SmallArray_GameType_Insert(SmallArray<GameTypeWithTypeInfo>& arrInOut, uint16 idx)
{
    GameTypeWithTypeInfo* newEnd = SmallArray_GameType_Append(arrInOut);
    GameTypeWithTypeInfo* targetPlace = &arrInOut[idx];
    for (GameTypeWithTypeInfo* elem = newEnd - 1; elem >= targetPlace; elem--)
    {
        *(elem + 1) = std::move(*elem);
    }
    ACUConstruct(*targetPlace);
    return targetPlace;
}
template<typename ElementType>
void SmallArrayAppend(SmallArray<ElementType>& arrInOut, const ElementType& newElem)
{
    // Implementation from `SmallArray_DWORD::AppendIfNotFound()` at 0000000140B8E8F0.

    uint16 newCap = 0x3FFF;
    uint16 oldCap = arrInOut.capacityAndFlags & 0x3FFF;
    if (arrInOut.size + 1 > oldCap)
    {
        if (oldCap == 0x3FFF)
        {
            // Max capacity, cannot increase. Crash on purpose.
            *(int*)0 = 0;
        }
        if (oldCap >= 0x18)
            newCap = oldCap / 3u + oldCap;
        else
            newCap = oldCap + 4;
        void* g_currentHeap_mb = *(void**)0x1452585D0;
        SmallArrayReserve(arrInOut, newCap, g_currentHeap_mb);
    }
    uint16 newSize = arrInOut.size + 1;
    arrInOut.size = newSize;
    arrInOut.arr[newSize - 1] = newElem;
}
template<typename ElementType>
void SmallArrayInsert(SmallArray<ElementType>& arr, ElementType newElem, uint16 insertWhere)
{
    void* g_currentHeap_mb = *(void**)0x1452585D0;
    SmallArrayReserve(arr, arr.size + 1, g_currentHeap_mb);
    arr.size++;
    for (uint16 i = arr.size - 1; i > insertWhere; i--)
    {
        arr[i] = arr[i - 1];
    }
    arr[insertWhere] = newElem;
}

} // namespace ACU::Memory

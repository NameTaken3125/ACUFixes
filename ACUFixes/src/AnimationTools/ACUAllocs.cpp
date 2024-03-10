#include "pch.h"

#include "ACUAllocs.h"

#include "ACU_DefineNativeFunction.h"


DEFINE_GAME_FUNCTION(Allocate_mb, 0x1426F3760, void*, __fastcall, (__int64 p_numBytes, __int64 p_alignment_mb, void* currentHeap_mb));
DEFINE_GAME_FUNCTION(AllocString_mb_orBytes, 0x140004510, void*, __fastcall, (__int64 p_numBytes, __int64 p_alignment_mb, void* resultOf0x1426F3760));
DEFINE_GAME_FUNCTION(MemoryManager__GetSomeFlagsBeforeDealloc, 0x142102740, __int64, __fastcall, (__int64 a1, byte* allocated));


byte* ACUAllocateBytes(uint32 numBytes, uint32 alignment)
{
    void* g_currentHeap_mb = *(void**)0x1452585D0;
    void* res1 = Allocate_mb(numBytes, alignment, g_currentHeap_mb);
    void* res2 = AllocString_mb_orBytes(numBytes, alignment, res1);
    return (byte*)res2;
}
namespace ACU::Memory
{
// Every allocation and deallocation takes 2 calls. What the hell is going on here?
class SomeOTHERMemoryManager
{
public:
    // @vtbl
    virtual void Unk_000();
    virtual void Unk_008();
    virtual void Unk_010();
    virtual void Unk_018();
    virtual void Unk_020();
    virtual void Unk_028_Deallocate(byte* allocated, __int64 resultOf0x142102740);

    // @helper_functions
    static SomeOTHERMemoryManager* GetSingleton() { return **(SomeOTHERMemoryManager***)0x0000000145221CF8; }
};
} // namespace ACU::Memory

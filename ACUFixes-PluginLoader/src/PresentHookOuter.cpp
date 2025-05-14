#include "pch.h"

#include "base.h"
#include "PresentHookOuter.h"
#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

#include "ACU/HasSwapchain.h"
#include "ACU_DefineNativeFunction.h"
// A full replacement leaves no allocated code in the call stack. This helps
// the CrashLog generate a stacktrace.
__int64 SwapchainPresentP_FullReplacement(HasSwapchain_6f8* hasSwapchain)
{
    struct Stack142305620
    {
        __int64 qword_0;
        __int64 qword_8;
        __int64 qword_10;
        byte byte_18;
        char pad_19[0x40 - 0x19];
    };
    assert_sizeof(Stack142305620, 0x40);
    using Present_fnt = HRESULT(PRESENT_CALL*)(IDXGISwapChain* thisptr, UINT SyncInterval, UINT Flags);
    DEFINE_GAME_FUNCTION(sub_1420FB6D0, 0x1420FB6D0, __int64, __fastcall, (Stack142305620 * a1, char a2));
    DEFINE_GAME_FUNCTION(sub_1423032D0, 0x1423032D0, __int64, , ());
    DEFINE_GAME_FUNCTION(sub_142303390, 0x142303390, int, __fastcall, (__int64 a1, HasSwapchain_6f8_0 * a2));
    DEFINE_GAME_FUNCTION(sub_142129CC0, 0x142129CC0, void*, , ());
    DEFINE_GAME_FUNCTION(sub_1421027B0, 0x1421027B0, __int64, __fastcall, (Stack142305620 * a1, void* a2));
    DEFINE_GAME_FUNCTION(sub_1421027F0, 0x1421027F0, __int64, __fastcall, (Stack142305620 * a1));
    Stack142305620 stackVar;
    sub_1420FB6D0(&stackVar, 1);

    Base::Hooks::GrabGraphicsDevicesInitializeImGuiAndDraw(hasSwapchain->swapchain);

    hasSwapchain->swapchain->Present(hasSwapchain->swapchainPresent_syncInterval, 0);
    sub_142303390(sub_1423032D0(), hasSwapchain->p0);
    __int64 v4 = sub_1421027B0(&stackVar, sub_142129CC0());
    stackVar.byte_18 = 0;
    stackVar.qword_8 += v4;
    stackVar.qword_10 = v4;
    __int64 v5 = sub_1421027F0(&stackVar);
    *(__int64*)0x14522BF08 = v5;
    return v5;
}

class PresentHookOuterPatch : public AutoAssemblerCodeHolder_Base
{
public:
    PresentHookOuterPatch();
};
PresentHookOuterPatch::PresentHookOuterPatch()
{
    PresetScript_ReplaceFunctionAtItsStart(0x142305620, SwapchainPresentP_FullReplacement);
}

static std::optional<AutoAssembleWrapper<PresentHookOuterPatch>> g_PresentOuterPatch;

void PresentHookOuter::Deactivate()
{
    if (g_PresentOuterPatch)
    {
        g_PresentOuterPatch->Deactivate();
    }
    // The global is deliberately not destroyed.
    // One of the methods of detaching is by clicking a button in ImGui itself.
    // If I do that, then the following things happen:
    //   1. The `.reset()` is called below;
    //   2. the destructor of the patch is called;
    //   3. the trampoline memory is freed;
    //   4. `BeforePresentIsCalledInGame()` finishes running;
    //   5. then returns to the exiting part of the trampoline;
    //   6. executes the part that restores all registers;
    //   7. then executes stolen bytes (still the same trampoline);
    //   8. then returns to the call site.
    // Except the trampoline was just freed, so steps 6-8 are actually a crash.
    // I think there might be a way to do this safely and avoid a race condition,
    // perhaps by using exceptions (SEH or VEH), or perhaps using the SafetyHook Project's
    // suspend-threads-and-manually-edit-RIP-method.

    // g_PresentOuterPatch.reset();
}
void PresentHookOuter::Activate()
{
    g_PresentOuterPatch.emplace();
    g_PresentOuterPatch->Activate();
}

#include "pch.h"

#include "base.h"
#include "PresentHookOuter.h"
#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

#include "ACU/HasSwapchain.h"
#include "ACU_DefineNativeFunction.h"

HRESULT PRESENT_CALL SwapchainPresentCallReplacement(IDXGISwapChain* thisptr, UINT SyncInterval)
{
    Base::Hooks::GrabGraphicsDevicesInitializeImGuiAndDraw(thisptr);
    const UINT Flags = 0;
    return thisptr->Present(SyncInterval, Flags);
}

class PresentHookOuterPatch : public AutoAssemblerCodeHolder_Base
{
public:
    PresentHookOuterPatch();
};
PresentHookOuterPatch::PresentHookOuterPatch()
{
    DEFINE_ADDR(justBeforePresentIsCalled, 0x14230563F);
    ALLOC(justBeforePresentIsCalled_cave, 0x80, 0x14230563F);

    // I execute a "call" instead of "jmp" to the code cave, from which I "jmp" into C++.
    // This way the return address pushed to the stack belongs to ACU.exe module
    // and has a corresponding unwind information, which helps the CrashLog in generating a stacktrace.

    // Total 6 bytes stolen; my custom Present()
    // will return to 0x142305644.
    justBeforePresentIsCalled = {
        db(0xE8), RIP(justBeforePresentIsCalled_cave),
        nop(1)
    };
    justBeforePresentIsCalled_cave = {
        "ff 25 00 00 00 00"                                 //  - jmp    QWORD PTR [rip+0]
        , dq((uintptr_t)SwapchainPresentCallReplacement)
    };
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

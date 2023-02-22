#include "pch.h"

#include "base.h"
#include "PresentHookOuter.h"
#include "AutoAssemblerKinda.h"

void BeforePresentIsCalledInGame(AllRegisters* params)
{
    Base::Hooks::GrabGraphicsDevicesInitializeImGuiAndDraw((IDXGISwapChain*)params->rcx_);
}


class PresentHookOuterPatch : public AutoAssemblerCodeHolder_Base
{
public:
    PresentHookOuterPatch();
};
PresentHookOuterPatch::PresentHookOuterPatch()
{
    uintptr_t beforePresentIsCalledInGame = 0x14230563F;
    PresetScript_CCodeInTheMiddle(
        beforePresentIsCalledInGame, 6,
        BeforePresentIsCalledInGame,
        RETURN_TO_RIGHT_AFTER_STOLEN_BYTES,
        true
    );
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

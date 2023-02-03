#include "pch.h"

#include "AutoAssemblerKinda.h"
#include "Hack_EnterWindowsWhenRisPressed.h"

void DrawAssemblerContextDebug(AssemblerContext& ctx)
{
    debug_GET_AA_SYMBOL(ctx, isRequestedToEnterWindow);
    debug_GET_AA_SYMBOL(ctx, setRbuttonState);
    debug_GET_AA_SYMBOL(ctx, setRbuttonState_cave);
    ImGui::Text("isRequestedToEnterWindow: %llx: %s", isRequestedToEnterWindow->m_ResolvedAddr.value(), static_cast<WriteableSymbol*>(isRequestedToEnterWindow)->GetResultBytesString().c_str());
    ImGui::Text("setRbuttonState: %llx: %s", setRbuttonState->m_ResolvedAddr.value(), static_cast<WriteableSymbol*>(setRbuttonState)->GetResultBytesString().c_str());
    ImGui::Text("setRbuttonState_cave: %llx: %s", setRbuttonState_cave->m_ResolvedAddr.value(), static_cast<WriteableSymbol*>(setRbuttonState_cave)->GetResultBytesString().c_str());
}
// Started using async version because this one produces noticeable stutter during AssemblerContext::AllocateVariables().
void DrawHacksControls_notasync()
{
    static AutoAssembleWrapper<EnterWindowWhenRisPressed> instantiation;
    bool isActive = instantiation.IsActive();
    if (ImGui::Checkbox("Toggle AutoAssemble Test", &isActive))
    {
        instantiation.Toggle();
    }
    DrawAssemblerContextDebug(instantiation.debug_GetAssemblerContext());
}

// Async-constructing the AutoAssemblerWrapper<CodeHolderObject> is better, because all the VirtualAllocs
// produce a noticeable stutter on creation otherwise.
template<typename Ty>
class AsyncConstructed
{
public:
    Ty* m_CachedPtr = nullptr;
    std::optional<Ty> m_Instance;
    std::future<void> m_Future;
    std::optional<std::exception> m_Exception;
public:
    AsyncConstructed()
    {
        m_Future = std::async(std::launch::async, [&]() {
            try
            {
                m_Instance.emplace();
                m_CachedPtr = &m_Instance.value();
            }
            catch (const std::exception& e)
            {
                m_Exception = e;
            }
            });
    }
    Ty* get() { return m_CachedPtr; }
    std::exception* GetException()
    {
        return m_Exception ? &m_Exception.value() : nullptr;
    }
};
void InspectAllRegisters(AllRegisters* parameters)
{
    int noop = 0;
}

struct CCodeInTheMiddle_TEST : AutoAssemblerCodeHolder_Base
{
    CCodeInTheMiddle_TEST();
};
CCodeInTheMiddle_TEST::CCodeInTheMiddle_TEST()
{
    CCodeInTheMiddleFunctionPtr_t receiverFunc = &InspectAllRegisters;
    uintptr_t whereToInject = 0x141A4C641;
    constexpr size_t howManyBytesStolen = 5;
    const bool isNeedToExecuteStolenBytesAfterwards = true;
    PresetScript_CCodeInTheMiddle(
        whereToInject, howManyBytesStolen
        , receiverFunc
        , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
        , isNeedToExecuteStolenBytesAfterwards);
}
void DrawHacksControls()
{
    // WARNING: running 2+ of these in async has been known to create a race in AssemblerContext::AllocateVariables()
    //          (between `FindFreeBlockForRegion()` and `VirtualAlloc()`). This has been alleviated by adding 10 tries,
    //          which is a simplified version of what Cheat Engine is doing and is fine for now.

    static AsyncConstructed<AutoAssembleWrapper<EnterWindowWhenRisPressed>> enteringWindows;
    static AsyncConstructed<AutoAssembleWrapper<CCodeInTheMiddle_TEST>> ccodeInTheMiddle;

    if (auto* instance = enteringWindows.get())
    {
        bool isActive = instance->IsActive();
        if (ImGui::Checkbox("Enter windows by pressing R", &isActive))
        {
            instance->Toggle();
        }
        DrawAssemblerContextDebug(instance->debug_GetAssemblerContext());
    }
    else if (auto* exc = enteringWindows.GetException())
    {
        int noop = 0;
    }

    if (auto* instance = ccodeInTheMiddle.get())
    {
        bool isActive = instance->IsActive();
        if (ImGui::Checkbox("Inspect all registers", &isActive))
        {
            instance->Toggle();
        }
    }
    else if (auto* exc = ccodeInTheMiddle.GetException())
    {
        int noop = 0;
    }
}

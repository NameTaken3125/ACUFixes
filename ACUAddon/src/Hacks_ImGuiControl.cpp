#include "pch.h"

#include "AutoAssemblerKinda.h"
#include "Hack_EnterWindowsWhenRisPressed.h"

void DrawAssemblerContextDebug(AssemblerContext& ctx)
{
    g_assemblerContext = &ctx;
    debug_GET_AA_SYMBOL(isRequestedToEnterWindow);
    debug_GET_AA_SYMBOL(setRbuttonState);
    debug_GET_AA_SYMBOL(setRbuttonState_cave);
    ImGui::Text("isRequestedToEnterWindow: %llx: %s", isRequestedToEnterWindow->m_ResolvedAddr.value(), static_cast<WriteableSymbol*>(isRequestedToEnterWindow)->GetResultBytesString().c_str());
    ImGui::Text("setRbuttonState: %llx: %s", setRbuttonState->m_ResolvedAddr.value(), static_cast<WriteableSymbol*>(setRbuttonState)->GetResultBytesString().c_str());
    ImGui::Text("setRbuttonState_cave: %llx: %s", setRbuttonState_cave->m_ResolvedAddr.value(), static_cast<WriteableSymbol*>(setRbuttonState_cave)->GetResultBytesString().c_str());
    g_assemblerContext = nullptr;
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
// produce a noticeable stutter on creationg otherwise.
// But I cannot use it if there are at least two users of `g_assemblerContext`.
// The global needs to be removed.
template<typename Ty>
class AsyncConstructed
{
    std::optional<Ty> m_Instance;
    std::future<void> m_Future;
public:
    AsyncConstructed()
    {
        m_Future = std::async(std::launch::async, [&]() {
            m_Instance.emplace();
            });
    }
    Ty* get() { return m_Instance ? &m_Instance.value() : nullptr; }
};
#include "basic_types.h"
typedef struct {
    union {
        struct {
            float f0;
            float f1;
            float f2;
            float f3;
        };
        struct {
            double d0;
            double d1;
        };
        float fa[4];
        double da[2];
    };
} xmmreg, * pxmmreg;
struct AllRegisters
{
    char pad_0[0xA0];
    xmmreg XMM0;
    xmmreg XMM1;
    xmmreg XMM2;
    xmmreg XMM3;
    xmmreg XMM4;
    xmmreg XMM5;
    xmmreg XMM6;
    xmmreg XMM7;
    xmmreg XMM8;
    xmmreg XMM9;
    xmmreg XMM10;
    xmmreg XMM11;
    xmmreg XMM12;
    xmmreg XMM13;
    xmmreg XMM14;
    xmmreg XMM15;
    char pad_1A0[0x200 - 0x1A0];
    unsigned long long rbx_;
    unsigned long long rcx_;
    unsigned long long rdx_;
    unsigned long long rsi_;
    unsigned long long rdi_;
    unsigned long long* rax_;
    unsigned long long rbp_;
    unsigned long long r8_;
    unsigned long long r9_;
    unsigned long long r10_;
    unsigned long long r11_;
    unsigned long long r12_;
    unsigned long long r13_;
    unsigned long long r14_;
    unsigned long long r15_;
};
assert_offsetof(AllRegisters, XMM0, 0xA0);
assert_offsetof(AllRegisters, XMM15, 0x190);
assert_offsetof(AllRegisters, rbx_, 0x200);
void InspectAllRegisters(AllRegisters* parameters)
{
    int noop = 0;
}

/*
The freaking god-combo of AutoAssembler.
jmp newmem
-> store registers
-> call [externalFuncAddr] (with ptr to registers as parameter)
-> restore registers
-> execute stolen bytes
-> jmp return
*/
struct CCodeInTheMiddle
{
    CCodeInTheMiddle();
    using CCodeInTheMiddleFunctionPtr_t = void (*)(AllRegisters * parameters);
    void PresetScript_CCodeInTheMiddle(uintptr_t whereToInject, size_t howManyBytesStolen, CCodeInTheMiddleFunctionPtr_t receiverFunc, std::optional<uintptr_t> whereToReturn, bool isNeedToExecuteStolenBytesAfterwards);
};
void CCodeInTheMiddle::PresetScript_CCodeInTheMiddle(uintptr_t whereToInject, size_t howManyBytesStolen, CCodeInTheMiddleFunctionPtr_t receiverFunc, std::optional<uintptr_t> whereToReturn, bool isNeedToExecuteStolenBytesAfterwards)
{
    std::string symbolsBaseName = "injectAt_" + std::to_string(whereToInject);
    DEFINE_ADDR_NAMED(injectAt, symbolsBaseName, whereToInject);
    uintptr_t injectionReturnAddr = whereToReturn ? whereToReturn.value() : whereToInject + howManyBytesStolen;
    DEFINE_ADDR_NAMED(injection_return, symbolsBaseName + "__return", injectionReturnAddr);

    ALLOC_NAMED(newmem, symbolsBaseName + "__newmem", 0x1000, whereToInject);
    LABEL_NAMED(ccode_flattened, symbolsBaseName + "__ccode_flattened");
    LABEL_NAMED(cave_entrance, symbolsBaseName + "__cave_entrance");
    /*DEFINE_ADDR(injectAt, whereToInject);
    uintptr_t injectionReturnAddr = whereToReturn ? whereToReturn.value() : whereToInject + howManyBytesStolen;
    DEFINE_ADDR(injection_return, injectionReturnAddr);

    ALLOC(newmem, 0x1000, whereToInject);
    LABEL(ccode_flattened);
    LABEL(cave_entrance);*/

    injectAt = {
        0xE9, RIP(cave_entrance),
        nop(howManyBytesStolen - 5),
    };
    newmem = {
        PutLabel(ccode_flattened),
        "9C                  "              //  - pushfq 
        "50                  "              //  - push rax
        "48 8B C4            "              //  - mov rax,rsp
        "48 83 E4 F0         "              //  - and rsp,-10               // align down to 0x10-byte boundary
        "48 81 EC A0020000   "              //  - sub rsp,000002A0
        "0FAE 44 24 20       "              //  - fxsave [rsp+20]
        "48 89 9C 24 20020000"              //  - mov [rsp+00000220],rbx
        "48 89 8C 24 28020000"              //  - mov [rsp+00000228],rcx
        "48 89 94 24 30020000"              //  - mov [rsp+00000230],rdx
        "48 89 B4 24 38020000"              //  - mov [rsp+00000238],rsi
        "48 89 BC 24 40020000"              //  - mov [rsp+00000240],rdi
        "48 89 84 24 48020000"              //  - mov [rsp+00000248],rax
        "48 89 AC 24 50020000"              //  - mov [rsp+00000250],rbp
        "4C 89 84 24 58020000"              //  - mov [rsp+00000258],r8
        "4C 89 8C 24 60020000"              //  - mov [rsp+00000260],r9
        "4C 89 94 24 68020000"              //  - mov [rsp+00000268],r10
        "4C 89 9C 24 70020000"              //  - mov [rsp+00000270],r11
        "4C 89 A4 24 78020000"              //  - mov [rsp+00000278],r12
        "4C 89 AC 24 80020000"              //  - mov [rsp+00000280],r13
        "4C 89 B4 24 88020000"              //  - mov [rsp+00000288],r14
        "4C 89 BC 24 90020000"              //  - mov [rsp+00000290],r15
        "48 8D 4C 24 20      "              //  - lea rcx,[rsp+20]
        "FF 15 80000000      "              //  - call qword ptr [label_externalFuncAddrBelow]  - doesn't need RIP-adjustment, because that label goes immediately after the end of current function, and distance to it is known.
        "4C 8B BC 24 90020000"              //  - mov r15,[rsp+00000290]
        "4C 8B B4 24 88020000"              //  - mov r14,[rsp+00000288]
        "4C 8B AC 24 80020000"              //  - mov r13,[rsp+00000280]
        "4C 8B A4 24 78020000"              //  - mov r12,[rsp+00000278]
        "4C 8B 9C 24 70020000"              //  - mov r11,[rsp+00000270]
        "4C 8B 94 24 68020000"              //  - mov r10,[rsp+00000268]
        "4C 8B 8C 24 60020000"              //  - mov r9,[rsp+00000260]
        "4C 8B 84 24 58020000"              //  - mov r8,[rsp+00000258]
        "48 8B AC 24 50020000"              //  - mov rbp,[rsp+00000250]
        "48 8B BC 24 40020000"              //  - mov rdi,[rsp+00000240]
        "48 8B B4 24 38020000"              //  - mov rsi,[rsp+00000238]
        "48 8B 94 24 30020000"              //  - mov rdx,[rsp+00000230]
        "48 8B 8C 24 28020000"              //  - mov rcx,[rsp+00000228]
        "48 8B 9C 24 20020000"              //  - mov rbx,[rsp+00000220]
        "0FAE 4C 24 20       "              //  - fxrstor [rsp+20]
        "48 8B A4 24 48020000"              //  - mov rsp,[rsp+00000248]
        "58                  "              //  - pop rax
        "9D                  "              //  - popfq 
        "C3                  "              //  - ret
    //label_externalFuncAddrBelow:
        , dq((unsigned long long)receiverFunc),
        PutLabel(cave_entrance),
        "E8", RIP(ccode_flattened),         //  - call ccode_flattened
    };
    if (isNeedToExecuteStolenBytesAfterwards)
    {
        ByteVector&& currentBytes = injectAt.CopyCurrentBytes(howManyBytesStolen);
        newmem += {db(std::move(currentBytes.m_bytes))};
    }
    newmem += {
        0xE9, RIP(injection_return),        //  - jmp injection_return
    };
}
CCodeInTheMiddle::CCodeInTheMiddle()
{
    CCodeInTheMiddleFunctionPtr_t receiverFunc = &InspectAllRegisters;
    uintptr_t whereToInject = 0x141A4C618;
    constexpr size_t howManyBytesStolen = 8;
    std::optional<uintptr_t> whereToReturn;
    const bool isNeedToExecuteStolenBytesAfterwards = true;
    PresetScript_CCodeInTheMiddle(whereToInject, howManyBytesStolen, receiverFunc, whereToReturn, isNeedToExecuteStolenBytesAfterwards);
}
void DrawHacksControls()
{
    static AutoAssembleWrapper<EnterWindowWhenRisPressed> enteringWindows;
    if (auto* instance = &enteringWindows)
    {
        bool isActive = instance->IsActive();
        if (ImGui::Checkbox("Enter windows by pressing R", &isActive))
        {
            instance->Toggle();
        }
        DrawAssemblerContextDebug(instance->debug_GetAssemblerContext());
    }

    static AutoAssembleWrapper<CCodeInTheMiddle> ccodeInTheMiddle;
    if (auto* instance = &ccodeInTheMiddle)
    {
        bool isActive = instance->IsActive();
        if (ImGui::Checkbox("Inspect all registers", &isActive))
        {
            instance->Toggle();
        }
    }
}
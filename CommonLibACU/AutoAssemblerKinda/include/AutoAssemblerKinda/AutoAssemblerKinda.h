#pragma once


/*
This little project is created to assist translating Cheat Engine's AutoAssembler-generated
code for x64 into C++.
THIS IS NOT AN ASSEMBLER.


Example usage:
In Cheat Engine's Auto Assembler:
        define(address,"ACU.exe"+1A4C618)
        define(bytes,49 8B 47 28 4C 8D 45 80)

        [ENABLE]

        assert(address,bytes)

        // `allocated_variable` could come from a different script, which means
        // the distance from it to `maybeSkipParkourScanner` hook could be different every time,
        // which means the generated code that depends on this variable could be different every time.
        alloc(allocated_variable,8,"ACU.exe")
        allocated_variable:
          dq 0

        alloc(maybeSkipParkourScanner,$1000,"ACU.exe"+1A4C618)
        label(return)


        newmem:
          cmp [allocated_variable], 0
          je return
          mov rax,[r15+28]
          lea r8,[rbp-80]
          lea rdx,[rbp-60]
          movaps xmm3,xmm7
          mov rcx,[rax]
          mov rax,[rcx+000001F0]
          mov rcx,[r15+00000208]
          mov [rsp+28],rax
          mov [rsp+20],edi
          call ACU.exe+1858D0
          mov rax,[r15+28]
          lea rcx,[rbp-70]
          lea r9,[rbp-80]
          mov rax,[rax]
          mov [rsp+40],rcx
          mov rcx,[r15+00000208]
          mov rax,[rax+000001F0]
          lea r8,[rbp-30]
          lea rdx,[rbp-60]
          mov [rsp+38],rax
          mov [rsp+30],r12l
          mov [rsp+28],edi
          movss [rsp+20],xmm7
          call ACU.exe+185630
          jmp return

        address:
          jmp maybeSkipParkourScanner
          nop 3
        return:

        [DISABLE]

        address:
          db bytes
        dealloc(maybeSkipParkourScanner)
        dealloc(allocated_variable)

        {
        // ORIGINAL CODE - INJECTION POINT: ACU.exe+1A4C618

        ACU.exe+1A4C60F: 0F 2F FE              - comiss xmm7,xmm6
        ACU.exe+1A4C612: 0F 86 83 00 00 00     - jbe ACU.exe+1A4C69B
        // ---------- INJECTING HERE ----------
        ACU.exe+1A4C618: 49 8B 47 28           - mov rax,[r15+28]
        // ---------- DONE INJECTING  ----------
        ACU.exe+1A4C61C: 4C 8D 45 80           - lea r8,[rbp-80]
        ACU.exe+1A4C620: 48 8D 55 A0           - lea rdx,[rbp-60]
        }
The above, when activated, turns into the following code (copied from Cheat Engine's Disassembler Window):
        ACU.exe+1A4C618 - E9 E33959FE           - jmp 13FFE0000
        ACU.exe+1A4C61D - 0F1F 00               - nop dword ptr [rax]
    and
        13FFE0000 - 83 3D F9FF0000 00     - cmp dword ptr [13FFF0000],00
        13FFE0007 - 0F84 13C6A601         - je ACU.exe+1A4C620
        13FFE000D - 49 8B 47 28           - mov rax,[r15+28]
        13FFE0011 - 4C 8D 45 80           - lea r8,[rbp-80]
        13FFE0015 - 48 8D 55 A0           - lea rdx,[rbp-60]
        13FFE0019 - 0F28 DF               - movaps xmm3,xmm7
        13FFE001C - 48 8B 08              - mov rcx,[rax]
        13FFE001F - 48 8B 81 F0010000     - mov rax,[rcx+000001F0]
        13FFE0026 - 49 8B 8F 08020000     - mov rcx,[r15+00000208]
        13FFE002D - 48 89 44 24 28        - mov [rsp+28],rax
        13FFE0032 - 89 7C 24 20           - mov [rsp+20],edi
        13FFE0036 - E8 95581A00           - call ACU.exe+1858D0
        13FFE003B - 49 8B 47 28           - mov rax,[r15+28]
        13FFE003F - 48 8D 4D 90           - lea rcx,[rbp-70]
        13FFE0043 - 4C 8D 4D 80           - lea r9,[rbp-80]
        13FFE0047 - 48 8B 00              - mov rax,[rax]
        13FFE004A - 48 89 4C 24 40        - mov [rsp+40],rcx
        13FFE004F - 49 8B 8F 08020000     - mov rcx,[r15+00000208]
        13FFE0056 - 48 8B 80 F0010000     - mov rax,[rax+000001F0]
        13FFE005D - 4C 8D 45 D0           - lea r8,[rbp-30]
        13FFE0061 - 48 8D 55 A0           - lea rdx,[rbp-60]
        13FFE0065 - 48 89 44 24 38        - mov [rsp+38],rax
        13FFE006A - 44 88 64 24 30        - mov [rsp+30],r12l
        13FFE006F - 89 7C 24 28           - mov [rsp+28],edi
        13FFE0073 - F3 0F11 7C 24 20      - movss [rsp+20],xmm7
        13FFE0079 - E8 B2551A00           - call ACU.exe+185630
        13FFE007E - E9 9DC5A601           - jmp ACU.exe+1A4C620
This can be turned into the following C++ code:
        struct ParkourScannerHook : AutoAssemblerCodeHolder_Base
        {
            ParkourScannerHook();
        };
        ParkourScannerHook::ParkourScannerHook()
        {
            ALLOC(allocated_variable, 8, 0x140000000);
            allocated_variable = {
                dq(0)
            };
            DEFINE_ADDR(maybeSkipParkourScanner, 0x141A4C618);
            DEFINE_ADDR(__fn1401858D0, 0x1401858D0);
            DEFINE_ADDR(__fn140185630, 0x140185630);
            LABEL(maybeSkipParkourScanner__return);
            ALLOC(maybeSkipParkourScanner__cave, 0x1000, 0x141A4C618);

            maybeSkipParkourScanner = {
                0xE9, RIP(maybeSkipParkourScanner__cave),   // - jmp 13FFC0030                                  - needs to be adjusted - relative `jmp` between static and allocated addresses.
                "0F1F 00"                                   // - nop dword ptr [rax]
            };
            maybeSkipParkourScanner__cave = {
                "83 3D", RIP(allocated_variable, 5), "00"       // - cmp dword ptr [allocated_variable],00      - needs to be adjusted - relative `cmp` between two dynamic addresses; second parameter 5 == how many bytes until the end of the opcode.
                "0F84", RIP(maybeSkipParkourScanner__return),   // - je ACU.exe+1A4C620                         - needs to be adjusted - relative `je` between static and allocated addresses.
                "49 8B 47 28         "  // - mov rax,[r15+28]
                "4C 8D 45 80         "  // - lea r8,[rbp-80]
                "48 8D 55 A0         "  // - lea rdx,[rbp-60]
                "0F28 DF             "  // - movaps xmm3,xmm7
                "48 8B 08            "  // - mov rcx,[rax]
                "48 8B 81 F0010000   "  // - mov rax,[rcx+000001F0]
                "49 8B 8F 08020000   "  // - mov rcx,[r15+00000208]
                "48 89 44 24 28      "  // - mov [rsp+28],rax
                "89 7C 24 20         "  // - mov [rsp+20],edi
                "E8", RIP(__fn1401858D0), // - call ACU.exe+1858D0                                              - needs to be adjusted - relative `call` between static and allocated addresses.
                "49 8B 47 28         "  // - mov rax,[r15+28]
                "48 8D 4D 90         "  // - lea rcx,[rbp-70]
                "4C 8D 4D 80         "  // - lea r9,[rbp-80]
                "48 8B 00            "  // - mov rax,[rax]
                "48 89 4C 24 40      "  // - mov [rsp+40],rcx
                "49 8B 8F 08020000   "  // - mov rcx,[r15+00000208]
                "48 8B 80 F0010000   "  // - mov rax,[rax+000001F0]
                "4C 8D 45 D0         "  // - lea r8,[rbp-30]
                "48 8D 55 A0         "  // - lea rdx,[rbp-60]
                "48 89 44 24 38      "  // - mov [rsp+38],rax
                "44 88 64 24 30      "  // - mov [rsp+30],r12l
                "89 7C 24 28         "  // - mov [rsp+28],edi
                "F3 0F11 7C 24 20    "  // - movss [rsp+20],xmm7
                "E8", RIP(__fn140185630), // - call ACU.exe+185630                                              - needs to be adjusted - relative `call` between static and allocated addresses.
                "E9", RIP(maybeSkipParkourScanner__return),  // - jmp ACU.exe+1A4C620                           - needs to be adjusted - relative `call` between static and allocated addresses.
            };
        }
        void CreateAndActivatePatch()
        {
            static AutoAssembleWrapper<EnterWindowWhenRisPressed> parkourScannerPatch;
            parkourScannerPatch.Activate();
        }


The workflow I had in mind is the following:
- In Cheat Engine, write AutoAssembler code until you're satisfied with the results.
- Copy the generated bytecode, and without any additional processing paste it into C++ as a string.
- Look at the generated bytes, recognize and manually select which of them depend on specific locations
    where the code is allocated, then automatically adjust them for the real allocations.
    Typically these are:
        - the `jmp newmem`, `jmp return`. The code generated for these opcodes
          depends on the distance in memory between the opcode and the `jmp` destination;
        - the reads and writes to a variable allocated separately from the code using it;
        - the read and writes to absolute addresses of dynamic variables (happens sometimes).
- Activate() the C++ "script" object.


However, the most often used feature of this project for me has been
the much more straightforward "C++ Code in the Middle" script preset.

Cheat Engine now allows you to write C code in the AutoAssembler scripts
using the new {$ccode} blocks.

For example, consider the following Cheat Engine Script snippet:
        ...
        ...
        alloc(newmem,$1000,"ACU.exe"+1A4C618)

        label(code)
        label(return)

        newmem:
        {$ccode rbxProxy=rbx rcxProxy=rcx}

        {$asm}
        code:
          // Execute the stolen bytes
          mov rax,[r15+28]
          lea r8,[rbp-80]
          // Return from `newmem`
          jmp return

        address:
          jmp newmem
        return:
        ...
        ...
This would generate the following bytecode upon activation:
    - call ccode_wrapper_function
       - save all registers to stack
       - call actual_C_function
          - read and write the _stored_ registers
       - restore all registers from stack (including the modifications)
       - return from ccode_wrapper_function
    - execute stolen bytes
    - jmp return from newmem
I carried this idea over to this project in the form of
    `AutoAssemblerCodeHolder_Base::PresetScript_CCodeInTheMiddle()`
The function can be used from the `AutoAssemblerCodeHolder_Base` subclasses,
and results in a patch that effectively executes a chosen C++ function before a chosen opcode.
This covers a lot of use cases, since you can now modify register values from C++,
the hooks are very quick to set up and have very few limitations
regarding where they can be placed (there are some, since I don't use any assembler/disassembler):
- At least 5 bytes need to be stolen to make the jump
- The stolen bytes' opcodes must not depend on where they are located
  (again, because I'm blindly copying the stolen bytes, and I'm not using an assembler/disassembler)
  - This means, the stolen bytes cannot refer to relative addresses, so not one of, for example:
    - jmp <static address>
    - je <static address>
    - call <static address> (unless you choose the option to _not_ execute the stolen bytes)
  - `jmp rax`, `call [r15]` and such would be fine. The idea is that the instruction's bytes
    mustn't depend on its location.


Example:
    Bytes and opcodes were copied from Disassembler Window; comments describe
    whether a `PresetScript_CCodeInTheMiddle()` can be used _before_ (in place of) each opcode:
       141A4C691 - 0FB6 C0               - movzx eax,al                       // YES: 7 bytes, 2 opcodes will be stolen
       141A4C694 - 41 0F47 C6            - cmova eax,r14d                     // YES: 7 bytes, 2 opcodes will be stolen
       141A4C698 - 89 45 E0              - mov [rbp-20],eax                   // YES: 8 bytes, 2 opcodes will be stolen
       141A4C69B - 80 7C 24 70 00        - cmp byte ptr [rsp+70],00           // YES: 5 bytes, 1 opcode will be stolen
       141A4C6A0 - 0F84 C9000000         - je ACU.exe+1A4C76F                 // NO: the bytes of `je` instruction depend on where it is located
       141A4C6A6 - 49 8B 4F 28           - mov rcx,[r15+28]                   // YES: 7 bytes, 2 opcodes will be stolen
       141A4C6AA - 48 8B 01              - mov rax,[rcx]                      // YES: 11 bytes, 2 opcodes will be stolen
       141A4C6AD - 48 83 B8 F0010000 00  - cmp qword ptr [rax+000001F0],00    // YES: 8 bytes, 1 opcode will be stolen
       141A4C6B5 - 0F85 B4000000         - jne ACU.exe+1A4C76F                // NO: the bytes of `jne` instruction depend on where it is located
       141A4C6BB - 48 8B 88 F0010000     - mov rcx,[rax+000001F0]             // YES: 7 bytes, 1 opcode will be stolen
       141A4C6C2 - E8 396078FE           - call ACU.exe+1D2700                // NO: the bytes of `call` instruction depend on where it is located
       141A4C6C7 - 84 C0                 - test al,al                         // ! NO: at least 5 bytes, so 2 opcodes are required here, but the second opcode is `jbe <static address>`, which cannot be used
       141A4C6C9 - 0F86 A0000000         - jbe ACU.exe+1A4C76F                // NO: the bytes of `jbe` instruction depend on where it is located


    For any of the allowed opcodes (e.g. at address 141A4C694) you can now create a hook like so:
        void InspectAllRegistersDuringParkour(AllRegisters* params)
        {
            // Freely read and modify the registers here.
            params->r14_ = 55;
            ParkourData* parkourData = (ParkourData*)params->r15_;
            // And use all of C++.
            parkourData->member_28->p0->p1F0 = nullptr;
        }
        struct ParkourHook : AutoAssemblerCodeHolder_Base
        {
            ParkourHook()
            {
                uintptr_t onParkourUpdate = 0x141A4C694;
                const bool executeStolenBytes = true;
                PresetScript_CCodeInTheMiddle(
                    onParkourUpdate, 7,
                    InspectAllRegistersDuringParkour, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, executeStolenBytes);
            }
        };
        void CreateAndActivateParkourUpdateHook()
        {
            static AutoAssembleWrapper<ParkourHook> parkourUpdateHook;
            parkourUpdateHook.Activate();
        }


*/


#define DEFINE_ADDR(symbol, addr) StaticSymbol& symbol = m_ctx->MakeNew_Define(addr, #symbol);
#define DEFINE_ADDR_NAMED(symbol, name, addr) StaticSymbol& symbol = m_ctx->MakeNew_Define(addr, name);
#define ALLOC(symbol, size, preferredAddrOptional) AllocatedWriteableSymbol& symbol = m_ctx->MakeNew_Alloc(size, #symbol, preferredAddrOptional);
#define ALLOC_NAMED(symbol, name, size, preferredAddrOptional) AllocatedWriteableSymbol& symbol = m_ctx->MakeNew_Alloc(size, name, preferredAddrOptional);
#define LABEL(symbol) Label& symbol = m_ctx->MakeNew_Label(#symbol);
#define LABEL_NAMED(symbol, name) Label& symbol = m_ctx->MakeNew_Label(name);





#include <vector>
#include <memory>
#include <optional>
#include <variant>
#include <string_view>

namespace AutoAssemblerKinda {
typedef unsigned char		byte;		// 8 bits
} // namespace AutoAssemblerKinda





class db
{
public:
    db(unsigned __int8 singleValue) : m_Values({ singleValue }) {}
    db(std::vector<unsigned __int8>&& values) : m_Values(std::move(values)) {}
    std::vector<unsigned __int8> m_Values;
};
class dw
{
public:
    dw(unsigned __int16 singleValue) : m_Values({ singleValue }) {}
    std::vector<unsigned __int16> m_Values;
};
class dd
{
public:
    dd(unsigned __int32 singleValue) : m_Values({ singleValue }) {}
    dd(std::vector<unsigned __int32>&& values) : m_Values(std::move(values)) {}
    std::vector<unsigned __int32> m_Values;
};
class dq
{
public:
    dq(unsigned __int64 singleValue) : m_Values({ singleValue }) {}
    dq(std::vector<unsigned __int64>&& values) : m_Values(std::move(values)) {}
    std::vector<unsigned __int64> m_Values;
};
class nop {
public:
    nop(size_t howManyBytes = 1) : m_HowManyBytes(howManyBytes) {}
    size_t m_HowManyBytes;
};
class SymbolWithAnAddress;
class RIP
{
public:
    RIP(SymbolWithAnAddress& symbol, int howManyBytesUntilTheEndOfOpcodeIncludingThese4bytes = 4)
        : m_Symbol(symbol)
        , m_HowManyBytesUntilTheEndOfOpcodeIncludingThese4bytes(howManyBytesUntilTheEndOfOpcodeIncludingThese4bytes)
    {}
    std::reference_wrapper<SymbolWithAnAddress> m_Symbol;
    uint32_t m_HowManyBytesUntilTheEndOfOpcodeIncludingThese4bytes;
};
class ABS
{
public:
    ABS(SymbolWithAnAddress& symbol, int absoluteAddrWidth) : m_Symbol(symbol), m_AbsoluteAddrWidth(absoluteAddrWidth) {}
    std::reference_wrapper<SymbolWithAnAddress> m_Symbol;
    uint8_t m_AbsoluteAddrWidth;
};
class Label;
class LabelPlacement
{
public:
    LabelPlacement(Label& label) : m_Label(label) {}
    std::reference_wrapper<Label> m_Label;
};
inline LabelPlacement PutLabel(Label& label) { return LabelPlacement(label); }
class WriteableSymbol;
using CodeElement = std::variant <
    // Paste code byte by byte.
    AutoAssemblerKinda::byte
    , db
    , dw
    , dd
    , dq
    // A string of hex numbers that can be copied from Cheat Engine's generated bytecode (e.g. "48 C1 E8 30" for `shr rax,30`)
    , std::string_view
    // Address of a symbol (Absolute or RIP-Relative, that depends on which opcode it is a part of), to correct opcodes that refer to it.
    // For example: Cheat Engine might turn "jmp newmem" into "E9 87654321", and that would mean
    // that the real address of `newmem` is equal to (0x21436587 + 5 + <current_opcode_position>).
    //      (+5 is because a near relative `jmp` opcode is 5 bytes long, and the relative addressing is relative to the _next_ instruction)
    // So this opcode can be translated here into {"E9", RIP(newmem)}.
    // Another, kinda specific example: if you have a `some_variable` allocated at 784B0000 (note that it has a 4-byte address),
    // then "mov [some_variable],2D681820" ==
    //      13FFF0004 - C7 04 25 00004B78 2018682D - mov [784B0000],2D681820 { 2018181120,(0) }
    // which translates into {"C7 04 25", ABS(some_variable), "2018682D"}
    // The following example uses relative addressing again, for some_variable at near address 1784B0000
    //      13FFF0004 - C7 05 F2FF4B38 2018682D - mov [1784B0000],2D681820
    //      13FFF000E - ...
    // If disassembled, this sequence of bytes actually corresponds to `mov DWORD PTR [rip+0x384bfff2], 0x2d681820`.
    // (0x384bfff2 == <address of some_variable> - <address_of_next_instruction> == 0x1784B0000 - 0x13FFF000E)
    // Since I don't disassemble anything here, I do not know where the next instruction starts, and that means that I do not know
    // what exactly to subtract from 0x1784B0000, and therefore I do not know what to write at 13FFF0006 (which is where the 4-byte relative address needs to be written).
    // Some additional information needs to be provided to determine the correct offset. Here's some math:
    // The correct offset == 0x384bfff2
    //      == 0x1784B0000 - 0x13FFF000E
    //      == <address of variable> - <address of next instruction>
    //      == <address of variable> - (<address of where the offset needs to be written> + 8)
    // So the `8` here is that additional information needed - the number of bytes until the end of the current opcode.
    // Therefore, the correct translation of this particular opcode would be:
    //      {"C7 05", RIP(some_variable, 8), "2018682D"}
    , RIP, ABS
    // Label's position will be defined by putting it in the code elements array at needed position.
    , LabelPlacement
    // Just some number of nop bytes
    , nop
> ;









struct SymbolMentionTypeRelative
{
    uint32_t m_HowManyBytesUntilTheEndOfOpcodeIncludingThese4bytes;
};
struct SymbolMentionTypeAbsolute
{
    uint32_t m_AbsoluteAddressWidth;
};
using SymbolReferenceDetails = std::variant<SymbolMentionTypeRelative, SymbolMentionTypeAbsolute>;
class SymbolMention
{
public:
    SymbolMention(WriteableSymbol& symbol, int offsetFromSymbolStart, SymbolReferenceDetails referenceOption)
        : m_Symbol(symbol)
        , m_OffsetFromSymbolStart(offsetFromSymbolStart)
        , m_ReferenceOption(referenceOption)
    {}
    WriteableSymbol& m_Symbol;
    /*
    E.g:
    If m_Symbol is at address 0x70000000,
    and contains `jmp 70123456`, the instruction needs to be resolved to "E9 51341200" == `jmp <0x70123456 - (0x70000000+5)>`,
    so the resolved relative address would be 0x123451 and `m_OffsetFromSymbolStart` == 1 (first byte after E9).
    */
    int m_OffsetFromSymbolStart;
    SymbolReferenceDetails m_ReferenceOption;
};
struct ByteVector
{
    std::vector<AutoAssemblerKinda::byte> m_bytes;
    ByteVector() {}
    ByteVector(std::vector<AutoAssemblerKinda::byte>&& bytes) : m_bytes(std::move(bytes)) {}
};
class SymbolWithAnAddress
{
public:
    virtual ~SymbolWithAnAddress() {}
    SymbolWithAnAddress(std::optional<uintptr_t> m_ConstantAddress) : m_ResolvedAddr(m_ConstantAddress) {}
    SymbolWithAnAddress(const SymbolWithAnAddress& other) = delete;
    std::string m_SymbolName;
    std::optional<uintptr_t> m_ResolvedAddr;
    std::vector<SymbolMention> m_ReferencesToResolve;
    ByteVector CopyCurrentBytes(size_t numBytes);
};
class AssemblerContext;
class WriteableSymbol : public SymbolWithAnAddress
{
    friend AssemblerContext;
    AssemblerContext* m_ctx;
public:
    WriteableSymbol(std::optional<uintptr_t> m_ConstantAddress, AssemblerContext* ctx) : SymbolWithAnAddress(m_ConstantAddress), m_ctx(ctx) {}
    WriteableSymbol& operator=(const std::vector<CodeElement>& codeElements) { SetCodeElements(codeElements); return *this; }
    WriteableSymbol& operator+=(const std::vector<CodeElement>& codeElements) { AppendCodeElements(codeElements); return *this; }
    void SetCodeElements(const std::vector<CodeElement>& codeElements);
    void AppendCodeElements(const std::vector<CodeElement>& codeElements);
    std::string GetResultBytesString();
protected:
    void ProcessNewCodeElements(size_t idxToStartProcessingFrom);
    std::vector<CodeElement> m_codeElements;
    std::vector<AutoAssemblerKinda::byte> m_resultantCode;
    void Write();
};
class StaticSymbol : public WriteableSymbol
{
    friend AssemblerContext;
public:
    using WriteableSymbol::WriteableSymbol;
    using WriteableSymbol::operator=;
private:
    std::vector<AutoAssemblerKinda::byte> m_OriginalBytes;
    void Write();
    void Unwrite();
};
class AllocatedWriteableSymbol : public WriteableSymbol
{
    friend AssemblerContext;
public:
    using WriteableSymbol::WriteableSymbol;
    using WriteableSymbol::operator=;
    virtual ~AllocatedWriteableSymbol();
private:
    uint32_t m_SizeToAllocate;
    uintptr_t m_PreferredAddr;
    std::optional<uintptr_t> m_SuccessfulAllocBase;
private:
    using WriteableSymbol::Write;
};
class Label : public SymbolWithAnAddress
{
public:
    using SymbolWithAnAddress::SymbolWithAnAddress;
    WriteableSymbol* m_AssignedInWhichSymbol = nullptr;
    uint32_t m_Offset;
};
class AssemblerContext
{
private:
    std::vector<std::unique_ptr<SymbolWithAnAddress>> m_Symbols;
    std::vector<Label*> m_LabelsQuickAccess;
    std::vector<AllocatedWriteableSymbol*> m_AllocsQuickAccess;
    std::vector<StaticSymbol*> m_DefinesQuickAccess;
public:
    Label&
        MakeNew_Label(const std::string_view& symbolName);
    StaticSymbol&
        MakeNew_Define(uintptr_t addr, const std::string_view& symbolName);
    AllocatedWriteableSymbol&
        MakeNew_Alloc(uint32_t size, const std::string_view& symbolName, uintptr_t preferredAddr = 0);
    void AddSymbolRef(SymbolWithAnAddress& symbol, const SymbolMention& newRef);
    void AssignLabel(Label& label, WriteableSymbol& symbol, uint32_t offset);
    void AllocateVariables();
    void ResolveSymbolAddresses();
    void ResolveSymbolReferences();

    void WriteChanges();
    void Unwrite();

    SymbolWithAnAddress* GetSymbol(const std::string_view& symbolName);
};

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
// From https://en.wikipedia.org/wiki/FLAGS_register.
struct RFLAGS
{
    bool CF : 1;
    bool unused_bit_1 : 1;
    bool PF : 1;
    bool unused_bit_3 : 1;
    bool AF : 1;
    bool unused_bit_5 : 1;
    bool ZF : 1;
    bool SF : 1;
    bool TF : 1;
    bool IF : 1;
    bool DF : 1;
    bool OF : 1;
    unsigned char unused_bits_12_15 : 16 - 12;
    unsigned short unused_bits_16_31;
    unsigned int unused_bits_32_63;
};
static_assert(sizeof(RFLAGS) == 8);
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
    unsigned long long* rax_;   // Note that RAX is accessed differently from other registers: the _value_ of RAX register is `*this->rax_`, not `this->rax_`.
    unsigned long long rbp_;
    unsigned long long r8_;
    unsigned long long r9_;
    unsigned long long r10_;
    unsigned long long r11_;
    unsigned long long r12_;
    unsigned long long r13_;
    unsigned long long r14_;
    unsigned long long r15_;
    unsigned long long GetRSP() { return (unsigned long long)rax_ + 0x18; }
    unsigned long long& GetRAX() { return *rax_; }
    RFLAGS& GetFLAGS() { return *(RFLAGS*)((unsigned long long)rax_ + 8); }
};
static_assert(offsetof(AllRegisters, XMM0) == 0xA0);
static_assert(offsetof(AllRegisters, XMM15) == 0x190);
static_assert(offsetof(AllRegisters, rbx_) == 0x200);
class AutoAssemblerCodeHolder_Base
{
public:
    std::unique_ptr<AssemblerContext> m_ctx;
public:
    AutoAssemblerCodeHolder_Base();
    // Can optionally be overridden.
    virtual void OnBeforeActivate() {}
    virtual void OnBeforeDeactivate() {}

    using CCodeInTheMiddleFunctionPtr_t = void (*)(AllRegisters* parameters);
    static std::optional<uintptr_t> RETURN_TO_RIGHT_AFTER_STOLEN_BYTES;
    /*
    The freaking god-combo of AutoAssembler.
    jmp newmem
    -> store registers
    -> call [externalFuncAddr] (with ptr to registers as parameter)
    -> restore registers
    -> execute stolen bytes
    -> jmp return
    */
    void PresetScript_CCodeInTheMiddle(
        uintptr_t whereToInject, size_t howManyBytesStolen
        , CCodeInTheMiddleFunctionPtr_t receiverFunc
        , std::optional<uintptr_t> whereToReturn = RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, bool isNeedToExecuteStolenBytesAfterwards = true);
    void PresetScript_NOP(
        uintptr_t whereToInject, size_t howManyBytesToNOP);
};
template<class HasAutoAssemblerCodeInConstructor>
class AutoAssembleWrapper
{
    static_assert(std::is_base_of_v<AutoAssemblerCodeHolder_Base, HasAutoAssemblerCodeInConstructor >,
        "AutoAssembleWrapper template parameter needs to be derived from `AutoAssemblerCodeHolder_Base`");
private:
    HasAutoAssemblerCodeInConstructor m_CodeHolderInstantiation;
public:
    AutoAssembleWrapper()
        : m_CodeHolderInstantiation()
    {
        m_CodeHolderInstantiation.m_ctx->AllocateVariables();
        m_CodeHolderInstantiation.m_ctx->ResolveSymbolAddresses();
        m_CodeHolderInstantiation.m_ctx->ResolveSymbolReferences();
    }
    void Activate()
    {
        if (m_IsActive) { return; }
        m_CodeHolderInstantiation.OnBeforeActivate();
        m_CodeHolderInstantiation.m_ctx->WriteChanges();
        m_IsActive = true;
    }
    void Deactivate()
    {
        if (!m_IsActive) { return; }
        m_CodeHolderInstantiation.OnBeforeDeactivate();
        m_CodeHolderInstantiation.m_ctx->Unwrite();
        m_IsActive = false;
    }
    void Toggle(bool activate)
    {
        if (activate)
            Activate();
        else
            Deactivate();
    }
    void Toggle()
    {
        Toggle(!m_IsActive);
    }
    bool IsActive() { return m_IsActive; }
    ~AutoAssembleWrapper()
    {
        if (m_IsActive) { Deactivate(); }
    }

    AssemblerContext& debug_GetAssemblerContext() { return *m_CodeHolderInstantiation.m_ctx; }
private:
    bool m_IsActive = false;
};

#define debug_GET_AA_SYMBOL(assemblerCtxReference, symbol) SymbolWithAnAddress* symbol = assemblerCtxReference.GetSymbol(#symbol);

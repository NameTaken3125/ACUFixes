#include "pch.h"

#include "AutoAssemblerKinda.h"
#include "Hack_EnterWindowsWhenRisPressed.h"



uint32_t float_bytes(float f) { return (uint32_t&)f; }
void Patch_RunWindowEntryTesterIfRequested_asmonly(SymbolWithAnAddress& isRequestedToEnterWindow, AssemblerContext* m_ctx);
void Patch_RunWindowEntryTesterIfRequested_cppTrampoline(SymbolWithAnAddress& isRequestedToEnterWindow, AssemblerContext* m_ctx);
void Patch_RememberRbuttonState(AllocatedWriteableSymbol& isRequestedToEnterWindow, AssemblerContext* m_ctx);
void Patch_AlwaysRunWindowEntryTester(AssemblerContext* m_ctx);
void Patch_FakeIsMovingFlag(AssemblerContext* m_ctx);
void Patch_AllowEnteringWindowsWithoutLeaningIntoThem(AllocatedWriteableSymbol& isRequestedToEnterWindow, AssemblerContext* m_ctx);
EnterWindowWhenRisPressed::EnterWindowWhenRisPressed()
{
    /*
    WARNING:
    It is best, actually very important sometimes, to keep allocations close to each other (as well as to the game code).
    Under very special circumstances allocating a variable without specifying a preferred address can cause very wrong code
    to be generated here.
    Example:        13FFC0000 - cmp byte ptr [isRequestedToEnterWindow],00
    if `isRequestedToEnterWindow` is allocated without specifying a preferred address, the following things can happen:
        &isRequestedToEnterWindow == 0x033E0000:        80 3C 25 00003E03 00 (absolute __4__byte__address__ (!) read)
        &isRequestedToEnterWindow == 0x119570000:       80 3D F9FF5AD9 00    (RIP-relative read)
        &isRequestedToEnterWindow == 0xAB000000:        FAILURE TO ASSEMBLE
    Very rarely (I only had it once) this can happen in the scripts of the Cheat Engine itself.
    What this means is that if the variable isn't allocated close enough to the assembly code using it,
    and I blindly copy the bytes from Cheat Engine, then it will _sometimes_ happen that neither Absolute nor RIP-Relative symbol reference
    will produce valid code (specifically, if I follow the Absolute reference example and write bytes "80 3C 25 000000AB 00" for 0xAB000000,
    the bytes will actually disassemble to `cmp [0xffffffffab000000],0x0`, which is 100% a read access violation).

    So when allocating variables, allocate them near the Game Module Base (0x140000000, usually) (do the same in Cheat Engine scripts).
    */
    ALLOC(isRequestedToEnterWindow, 8, 0x140000000);
    isRequestedToEnterWindow = {
        dq(0)
    };
    Patch_RememberRbuttonState(isRequestedToEnterWindow, m_ctx.get());
    Patch_AlwaysRunWindowEntryTester(m_ctx.get());
    Patch_FakeIsMovingFlag(m_ctx.get());
    Patch_RunWindowEntryTesterIfRequested_cppTrampoline(isRequestedToEnterWindow, m_ctx.get());
    Patch_AllowEnteringWindowsWithoutLeaningIntoThem(isRequestedToEnterWindow, m_ctx.get());
}
void Patch_AllowEnteringWindowsWithoutLeaningIntoThem(AllocatedWriteableSymbol& isRequestedToEnterWindow, AssemblerContext* m_ctx)
{

    /*
    allowEnterWindowsWithoutLeaningIntoThem__cave:
    13FFB0000 - 80 3D F9FF0300 00     - cmp byte ptr [isRequestedToEnterWindow],00
    13FFC0008 - 0F84 20000000         - je 13FFC002E
    13FFC000E - 48 B8 E00FA74301000000 - mov rax,ACU.exe+3A70FE0
    13FFC0018 - 48 39 41 08           - cmp [rcx+08],rax
    13FFC001C - 0F85 0C000000         - jne 13FFC002E
    13FFC0022 - E8 D92F2100           - call ACU.exe+1D3000
    13FFC0027 - 3C 02                 - cmp al,02
    13FFC0029 - E9 093D1700           - jmp ACU.exe+133D37
    13FFC002E - E8 CD2F2100           - call ACU.exe+1D3000
    13FFC0033 - 84 C0                 - test al,al
    13FFC0035 - E9 FD3C1700           - jmp ACU.exe+133D37
    */
    DEFINE_ADDR(allowEnterWindowsWithoutLeaningIntoThem, 0x140133D30);
    DEFINE_ADDR(allowEnterWindowsWithoutLeaningIntoThem__return, 0x140133D37);
    DEFINE_ADDR(_fn1401D3000, 0x1401D3000);
    ALLOC(allowEnterWindowsWithoutLeaningIntoThem__cave, 0x1000, 0x140133D30);
    allowEnterWindowsWithoutLeaningIntoThem = {
        0xE9, RIP(allowEnterWindowsWithoutLeaningIntoThem__cave)
    };
    allowEnterWindowsWithoutLeaningIntoThem__cave = {
        "80 3D", RIP(isRequestedToEnterWindow, 5), "00" //- cmp byte ptr [isRequestedToEnterWindow],00
        "0F84 20000000                  "               //- je 13FFC002E
        "48 B8 E00FA74301000000         "               // - mov rax,ACU.exe+3A70FE0
        "48 39 41 08                    "               //- cmp [rcx+08],rax
        "0F85 0C000000                  "               //- jne 13FFC002E
        "E8", RIP(_fn1401D3000),                        //- call ACU.exe+1D3000
        "3C 02                          "               //- cmp al,02
        "E9", RIP(allowEnterWindowsWithoutLeaningIntoThem__return),                  //- jmp ACU.exe+133D37
        "E8", RIP(_fn1401D3000),                        //- call ACU.exe+1D3000
        "84 C0                          "               //- test al,al
        "E9", RIP(allowEnterWindowsWithoutLeaningIntoThem__return)                   //- jmp ACU.exe+133D37
    };
}
void Patch_FakeIsMovingFlag(AssemblerContext* m_ctx)
{
    DEFINE_ADDR(fake_isMoving_flag, 0x141A4CBDC);
    DEFINE_ADDR(fake_isMoving_flag__return, 0x141A4CBDC + 7);
    ALLOC(fake_isMoving_flag__cave, 0x100, 0x141A4CBDC);
    fake_isMoving_flag = {
        0xE9, RIP(fake_isMoving_flag__cave),
        nop(2)
    };
    /*
    13FFE0000 - C6 85 E0000000 01     - mov byte ptr [rbp+000000E0],01
    13FFE0007 - 80 BD E0000000 00     - cmp byte ptr [rbp+000000E0],00
    13FFE000E - E9 D0CBA601           - jmp ACU.exe+1A4CBE3
    */
    fake_isMoving_flag__cave = { R"(
        C6 85 E0000000 01
        80 BD E0000000 00
        E9)", RIP(fake_isMoving_flag__return)
    };
}
void Patch_AlwaysRunWindowEntryTester(AssemblerContext* m_ctx)
{
    DEFINE_ADDR(alwaysRunWindowEntryTester, 0x141A4C612);
    alwaysRunWindowEntryTester = {
        nop(6)
    };
}
void Patch_RememberRbuttonState(AllocatedWriteableSymbol& isRequestedToEnterWindow, AssemblerContext* m_ctx)
{
    DEFINE_ADDR(setRbuttonState, 0x142739C90);
    LABEL(setRbuttonState_return);
    ALLOC(setRbuttonState_cave, 0x1000, 0x142739C90);

    setRbuttonState = {
        "E9", RIP(setRbuttonState_cave),
        nop(3),
        PutLabel(setRbuttonState_return),
    };
    setRbuttonState_cave =
    {
        "48 C1 E8 30"
        "A2", ABS(isRequestedToEnterWindow, 8),
        "48 8B 42 40"
        "48 89 43 40"
        "E9", RIP(setRbuttonState_return),
    };
}
#define DEFINE_GAME_FUNCTION(FuncName, relativeOffset, returnType, callingConvention, allParamsInParentheses) \
using FuncName##_t = returnType(callingConvention*)allParamsInParentheses;\
FuncName##_t FuncName = (FuncName##_t)relativeOffset;

#include "ACU/Parkour_PotentialWindowEntry.h"
#include "ACU/SmallArray.h"
class WindowEntryTester;
DEFINE_GAME_FUNCTION(WindowEntryTester__InitializeForFrame_mb, 0x1401858D0, int, __fastcall, (WindowEntryTester* a1, Vector4f* p_handsPosition, Vector4f* p_movementDirInGroundPlane, float p_WASDmagnitude, int p_eq6fullLean, __int64 p_currentLedge_mb));
DEFINE_GAME_FUNCTION(FindMatchingParkourActionsForCurrentMovement_P, 0x140185630, __int64, __fastcall, (WindowEntryTester* a1, Vector4f* p_handsPosition, Vector4f* p_towardWallCoplanarWithPlayerEntity, Vector4f* p_movementDirInGroundPlane, float p_WASDmagnitude, int p_eq6_fullLean, unsigned __int8 a7, __int64 p_currentLedge_mb, SmallArray<PotentialWindowEntry*>* p_arrayPotentialMovesOut));
void RunWindowEntryEntryTesterInitAndScan(
    WindowEntryTester* windowEntryTester
    , Vector4f* p_handsPosition
    , Vector4f* p_movementDirInGroundPlane
    , float p_WASDmagnitude
    , int p_eq6fullLean
    , __int64 p_currentLedge_mb
    , Vector4f* p_towardWallCoplanarWithPlayerEntity
    , unsigned __int8 a7
    , SmallArray<PotentialWindowEntry*>* p_arrayPotentialMovesOut
)
{
    constexpr int VKEY_R = 0x52;
    const bool isRequestedEnterWindow = GetAsyncKeyState(VKEY_R);
    if (!isRequestedEnterWindow)
    {
        // Standard execution
        WindowEntryTester__InitializeForFrame_mb(windowEntryTester, p_handsPosition, p_movementDirInGroundPlane, p_WASDmagnitude, p_eq6fullLean, p_currentLedge_mb);
        FindMatchingParkourActionsForCurrentMovement_P(windowEntryTester, p_handsPosition, p_towardWallCoplanarWithPlayerEntity, p_movementDirInGroundPlane, p_WASDmagnitude, p_eq6fullLean, a7, p_currentLedge_mb, p_arrayPotentialMovesOut);
        return;
    }
    [&]() {
        // Requested to enter window if one is nearby.
        std::array<Vector4f, 4> directionsToScan = {
            Vector4f{1, 0, 0, 0},
            Vector4f{-1, 0, 0, 0},
            Vector4f{0, 1, 0, 0},
            Vector4f{0, -1, 0, 0},
        };
        float fakeMovementMagnitude = 1;
        for (Vector4f& direction : directionsToScan)
        {
            WindowEntryTester__InitializeForFrame_mb(windowEntryTester, p_handsPosition, &direction, fakeMovementMagnitude, p_eq6fullLean, p_currentLedge_mb);
            FindMatchingParkourActionsForCurrentMovement_P(windowEntryTester, p_handsPosition, p_towardWallCoplanarWithPlayerEntity, &direction, fakeMovementMagnitude, p_eq6fullLean, a7, p_currentLedge_mb, p_arrayPotentialMovesOut);
        }
        int noop = 0;
    }();
}
void Patch_RunWindowEntryTesterIfRequested_cppTrampoline(SymbolWithAnAddress& isRequestedToEnterWindow, AssemblerContext* m_ctx)
{
    DEFINE_ADDR(runWindowEntrySearch, 0x141A4C618);
    DEFINE_ADDR(runWindowEntrySearch__return, 0x141A4C689);
    ALLOC(runWindowEntrySearch__cave, 0x1000, 0x141A4C618);
    LABEL(rwes_CPPfunction_addr);

    runWindowEntrySearch = {
        0xE9, RIP(runWindowEntrySearch__cave), // - jmp 13FFC0030
        "0F1F 00"                              // - nop dword ptr [rax]
    };
    runWindowEntrySearch__cave = {
        "48 83 EC 20        "  // - sub rsp,20
        "49 8B 47 28        "  // - mov rax,[r15+28]
        "4C 8D 45 80        "  // - lea r8,[rbp-80]
        "48 8D 55 A0        "  // - lea rdx,[rbp-60]
        "0F28 DF            "  // - movaps xmm3,xmm7
        "48 8B 08           "  // - mov rcx,[rax]
        "48 8B 81 F0010000  "  // - mov rax,[rcx+000001F0]
        "49 8B 8F 08020000  "  // - mov rcx,[r15+00000208]
        "48 89 44 24 28     "  // - mov [rsp+28],rax
        "89 7C 24 20        "  // - mov [rsp+20],edi
        "48 8D 45 D0        "  // - lea rax,[rbp-30]
        "48 89 44 24 30     "  // - mov [rsp+30],rax
        "44 88 64 24 38     "  // - mov [rsp+38],r12l
        "48 8D 45 90        "  // - lea rax,[rbp-70]
        "48 89 44 24 40     "  // - mov [rsp+40],rax
        "FF 15", RIP(rwes_CPPfunction_addr),    // - call [rip+rwes_CPPfunction_addr]
        "48 83 C4 20",                          // - add rsp,20
        0xE9, RIP(runWindowEntrySearch__return),
        PutLabel(rwes_CPPfunction_addr),
        dq((uintptr_t)&RunWindowEntryEntryTesterInitAndScan),
    };
}
void Patch_RunWindowEntryTesterIfRequested_asmonly(SymbolWithAnAddress& isRequestedToEnterWindow, AssemblerContext* m_ctx)
{

    /*
    141A4C618 - E9 133A57FE           - jmp 13FFC0030
    141A4C61D - 0F1F 00               - nop dword ptr [rax]

    13FFC0030 - 80 3D C9FF0200 00     - cmp byte ptr [isRequestedToEnterWindow],00
    13FFC0037 - 0F84 48020000         - je 13FFC0285
    13FFC003D - 48 8D 45 80           - lea rax,[rbp-80]
    13FFC0041 - 4C 8B 00              - mov r8,[rax]
    13FFC0044 - 4C 89 05 B5FFFFFF     - mov [13FFC0000],r8
    13FFC004B - 4C 8B 40 08           - mov r8,[rax+08]
    13FFC004F - 4C 89 05 B2FFFFFF     - mov [13FFC0008],r8
    13FFC0056 - 0F29 3D B3FFFFFF      - movaps [13FFC0010],xmm7
    13FFC005D - 0F28 3D BCFFFFFF      - movaps xmm7,[13FFC0020]

    13FFC0064 - C7 45 80 00000000     - mov [rbp-80],00000000
    13FFC006B - C7 45 84 000000C0     - mov [rbp-7C],C0000000
    { Call WindowEntryTester::InitializeForCurrentFrame() and WindowEntryTester::Scan()
    13FFC0072 - 49 8B 47 28           - mov rax,[r15+28]
    13FFC0076 - 4C 8D 45 80           - lea r8,[rbp-80]
    13FFC007A - 48 8D 55 A0           - lea rdx,[rbp-60]
    13FFC007E - 0F28 DF               - movaps xmm3,xmm7
    13FFC0081 - 48 8B 08              - mov rcx,[rax]
    13FFC0084 - 48 8B 81 F0010000     - mov rax,[rcx+000001F0]
    13FFC008B - 49 8B 8F 08020000     - mov rcx,[r15+00000208]
    13FFC0092 - 48 89 44 24 28        - mov [rsp+28],rax
    13FFC0097 - 89 7C 24 20           - mov [rsp+20],edi
    13FFC009B - E8 30581C00           - call ACU.exe+1858D0
    13FFC00A0 - 49 8B 47 28           - mov rax,[r15+28]
    13FFC00A4 - 48 8D 4D 90           - lea rcx,[rbp-70]
    13FFC00A8 - 4C 8D 4D 80           - lea r9,[rbp-80]
    13FFC00AC - 48 8B 00              - mov rax,[rax]
    13FFC00AF - 48 89 4C 24 40        - mov [rsp+40],rcx
    13FFC00B4 - 49 8B 8F 08020000     - mov rcx,[r15+00000208]
    13FFC00BB - 48 8B 80 F0010000     - mov rax,[rax+000001F0]
    13FFC00C2 - 4C 8D 45 D0           - lea r8,[rbp-30]
    13FFC00C6 - 48 8D 55 A0           - lea rdx,[rbp-60]
    13FFC00CA - 48 89 44 24 38        - mov [rsp+38],rax
    13FFC00CF - 44 88 64 24 30        - mov [rsp+30],r12l
    13FFC00D4 - 89 7C 24 28           - mov [rsp+28],edi
    13FFC00D8 - F3 0F11 7C 24 20      - movss [rsp+20],xmm7
    13FFC00DE - E8 4D551C00           - call ACU.exe+185630
    }
    13FFC00E3 - C7 45 80 00000000     - mov [rbp-80],00000000
    13FFC00EA - C7 45 84 00000040     - mov [rbp-7C],40000000
    { Call WindowEntryTester::InitializeForCurrentFrame() and WindowEntryTester::Scan() }
    13FFC0162 - C7 45 80 00000040     - mov [rbp-80],40000000
    13FFC0169 - C7 45 84 00000000     - mov [rbp-7C],00000000
    { Call WindowEntryTester::InitializeForCurrentFrame() and WindowEntryTester::Scan() }
    13FFC01E1 - C7 45 80 000000C0     - mov [rbp-80],C0000000
    13FFC01E8 - C7 45 84 00000000     - mov [rbp-7C],00000000
    { Call WindowEntryTester::InitializeForCurrentFrame() and WindowEntryTester::Scan() }
    13FFC0260 - 48 8D 45 80           - lea rax,[rbp-80]
    13FFC0264 - 4C 8B 05 95FDFFFF     - mov r8,[13FFC0000]
    13FFC026B - 4C 89 00              - mov [rax],r8
    13FFC026E - 4C 8B 05 93FDFFFF     - mov r8,[13FFC0008]
    13FFC0275 - 4C 89 40 08           - mov [rax+08],r8
    13FFC0279 - 0F28 3D 90FDFFFF      - movaps xmm7,[13FFC0010]
    13FFC0280 - E9 04C4A801           - jmp ACU.exe+1A4C689
    { Call WindowEntryTester::InitializeForCurrentFrame() and WindowEntryTester::Scan() }
    13FFC02F6 - E9 8EC3A801           - jmp ACU.exe+1A4C689
    */
    {
        DEFINE_ADDR(runWindowEntrySearch, 0x141A4C618);
        ALLOC(runWindowEntrySearch__cave, 0x1000, 0x141A4C618);
        LABEL(runWindowEntrySearch__cave__newCode);
        DEFINE_ADDR(runWindowEntrySearch__return, 0x141A4C689);
        DEFINE_ADDR(_WindowEntryTester__Initialize, 0x1401858D0);
        DEFINE_ADDR(_WindowEntryTester__DoScan, 0x140185630);
        runWindowEntrySearch = {
            0xE9, RIP(runWindowEntrySearch__cave__newCode), // - jmp 13FFC0030
            "0F1F 00"                              // - nop dword ptr [rax]
        };
        std::vector<CodeElement> macro__InitializeWindowEntryTesterAndDoScan = {
            "49 8B 47 28"            // - mov rax,[r15+28]
            "4C 8D 45 80"            // - lea r8,[rbp-80]
            "48 8D 55 A0"            // - lea rdx,[rbp-60]
            "0F28 DF"                // - movaps xmm3,xmm7
            "48 8B 08"               // - mov rcx,[rax]
            "48 8B 81 F0010000"      // - mov rax,[rcx+000001F0]
            "49 8B 8F 08020000"      // - mov rcx,[r15+00000208]
            "48 89 44 24 28"         // - mov [rsp+28],rax
            "89 7C 24 20"            // - mov [rsp+20],edi
            "E8", RIP(_WindowEntryTester__Initialize), // - call ACU.exe+1858D0
            "49 8B 47 28"            // - mov rax,[r15+28]
            "48 8D 4D 90"            // - lea rcx,[rbp-70]
            "4C 8D 4D 80"            // - lea r9,[rbp-80]
            "48 8B 00"               // - mov rax,[rax]
            "48 89 4C 24 40"         // - mov [rsp+40],rcx
            "49 8B 8F 08020000"      // - mov rcx,[r15+00000208]
            "48 8B 80 F0010000"      // - mov rax,[rax+000001F0]
            "4C 8D 45 D0"            // - lea r8,[rbp-30]
            "48 8D 55 A0"            // - lea rdx,[rbp-60]
            "48 89 44 24 38"         // - mov [rsp+38],rax
            "44 88 64 24 30"         // - mov [rsp+30],r12l
            "89 7C 24 28"            // - mov [rsp+28],edi
            "F3 0F11 7C 24 20"       // - movss [rsp+20],xmm7
            "E8", RIP(_WindowEntryTester__DoScan), // - call ACU.exe+185630
        };
        runWindowEntrySearch__cave = {
            dd({
            0, 0, 0, 0, // prevMovementDirection
            0, 0, 0, 0, // prevMovementMagnitude
            float_bytes(1), 0, 0, 0// fakeMovementMagnitudeXMM
                }),
            PutLabel(runWindowEntrySearch__cave__newCode),
            "80 3D", RIP(isRequestedToEnterWindow, 5), "00",      // - cmp byte ptr [isRequestedToEnterWindow],000
            "0F84 48020000"          // - je 13FFC0285
            "48 8D 45 80"            // - lea rax,[rbp-80]
            "4C 8B 00"               // - mov r8,[rax]
            "4C 89 05 B5FFFFFF"      // - mov [13FFC0000],r8
            "4C 8B 40 08"            // - mov r8,[rax+08]
            "4C 89 05 B2FFFFFF"      // - mov [13FFC0008],r8
            "0F29 3D B3FFFFFF"       // - movaps [13FFC0010],xmm7
            "0F28 3D BCFFFFFF"       // - movaps xmm7,[13FFC0020]
            "C7 45 80 00000000"      // - mov [rbp-80],00000000
            "C7 45 84 000000C0"      // - mov [rbp-7C],C0000000
        };
        runWindowEntrySearch__cave += macro__InitializeWindowEntryTesterAndDoScan;
        runWindowEntrySearch__cave += {
            "C7 45 80 00000000"      // - mov [rbp-80],00000000
                "C7 45 84 00000040"      // - mov [rbp-7C],40000000
        };
        runWindowEntrySearch__cave += macro__InitializeWindowEntryTesterAndDoScan;
        runWindowEntrySearch__cave += {
            "C7 45 80 00000040"      // - mov [rbp-80],40000000
                "C7 45 84 00000000"      // - mov [rbp-7C],00000000
        };
        runWindowEntrySearch__cave += macro__InitializeWindowEntryTesterAndDoScan;
        runWindowEntrySearch__cave += {
            "C7 45 80 000000C0"      // - mov [rbp-80],C0000000
                "C7 45 84 00000000"      // - mov [rbp-7C],00000000
        };
        runWindowEntrySearch__cave += macro__InitializeWindowEntryTesterAndDoScan;
        runWindowEntrySearch__cave += {
            "48 8D 45 80"            // - lea rax,[rbp-80]
                "4C 8B 05 95FDFFFF"      // - mov r8,[13FFC0000]
                "4C 89 00"               // - mov [rax],r8
                "4C 8B 05 93FDFFFF"      // - mov r8,[13FFC0008]
                "4C 89 40 08"            // - mov [rax+08],r8
                "0F28 3D 90FDFFFF"       // - movaps xmm7,[13FFC0010]
                "E9", RIP(runWindowEntrySearch__return),   // - jmp ACU.exe+1A4C689
        };
        runWindowEntrySearch__cave += macro__InitializeWindowEntryTesterAndDoScan;
        runWindowEntrySearch__cave += {
            "E9", RIP(runWindowEntrySearch__return),   // - jmp ACU.exe+1A4C689
        };
    }
}

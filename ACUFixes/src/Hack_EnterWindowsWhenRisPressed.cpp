#include "pch.h"

#include "AutoAssemblerKinda.h"
#include "Hack_EnterWindowsWhenRisPressed.h"

#include "ACU/Parkour_PotentialWindowEntry.h"
#include "ACU/SmallArray.h"
#include "ACU_InputUtils.h"

#include "MainConfig.h"

uint32_t float_bytes(float f) { return (uint32_t&)f; }
void Patch_RunWindowEntryTesterIfRequested_cppTrampoline(AssemblerContext* m_ctx);
void Patch_AlwaysRunWindowEntryTester(AssemblerContext* m_ctx);
void Patch_FakeIsMovingFlag(AssemblerContext* m_ctx);
void WhenCheckingIfWindowIsToBeEnteredNow_ConfirmIfRequestedForThisFrame(AllRegisters* params)
{
    if (IsPressed(enterWindowsButton))
    {
        *(uint32*)(params->rcx_ + 0xDC) = 6;
    }
}
#define DEFINE_GAME_FUNCTION(FuncName, relativeOffset, returnType, callingConvention, allParamsInParentheses) \
using FuncName##_t = returnType(callingConvention*)allParamsInParentheses;\
FuncName##_t FuncName = (FuncName##_t)relativeOffset;

DEFINE_GAME_FUNCTION(IsShouldEnterHideyClosetNow, 0x14265B350, char, __fastcall, (__int64 a1, __int64 a2));
void WhenCheckingIfHideyClosetIsToBeEnteredNow_ConfirmIfRequestedForThisFrame(AllRegisters* params)
{
    if (IsPressed(enterWindowsButton))
    {
        *params->rax_ = 1;
        return;
    }
    *params->rax_ = IsShouldEnterHideyClosetNow(params->rcx_, params->rdx_);
}
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
    Patch_AlwaysRunWindowEntryTester(m_ctx.get());
    Patch_FakeIsMovingFlag(m_ctx.get());
    Patch_RunWindowEntryTesterIfRequested_cppTrampoline(m_ctx.get());
    uintptr_t whenCheckingIfWindowIsToBeEnteredNow = 0x1401B0790;
    PresetScript_CCodeInTheMiddle(whenCheckingIfWindowIsToBeEnteredNow, 7,
        WhenCheckingIfWindowIsToBeEnteredNow_ConfirmIfRequestedForThisFrame, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenCheckingIfHideyClosetIsToBeEnteredNow = 0x14265A2DF;
    PresetScript_CCodeInTheMiddle(whenCheckingIfHideyClosetIsToBeEnteredNow, 5,
        WhenCheckingIfHideyClosetIsToBeEnteredNow_ConfirmIfRequestedForThisFrame, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
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
    const bool isRequestedEnterWindow = IsPressed(enterWindowsButton);
    if (!isRequestedEnterWindow)
    {
        // Standard execution
        WindowEntryTester__InitializeForFrame_mb(windowEntryTester, p_handsPosition, p_movementDirInGroundPlane, p_WASDmagnitude, p_eq6fullLean, p_currentLedge_mb);
        FindMatchingParkourActionsForCurrentMovement_P(windowEntryTester, p_handsPosition, p_towardWallCoplanarWithPlayerEntity, p_movementDirInGroundPlane, p_WASDmagnitude, p_eq6fullLean, a7, p_currentLedge_mb, p_arrayPotentialMovesOut);
        return;
    }
    [&]() {
        // Requested to enter window if one is nearby.
        std::array<Vector4f, 8> directionsToScan = {
            Vector4f{1, 0, 0, 0},
            Vector4f{-1, 0, 0, 0},
            Vector4f{0, 1, 0, 0},
            Vector4f{0, -1, 0, 0},
            // See 2nd story window in Franciade at [64.77, 71.73, 4.76]:
            // The four cardinal directions sadly aren't enough.
            Vector4f{1, 1, 0, 0},
            Vector4f{1, -1, 0, 0},
            Vector4f{-1, 1, 0, 0},
            Vector4f{-1, -1, 0, 0},
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
void Patch_RunWindowEntryTesterIfRequested_cppTrampoline(AssemblerContext* m_ctx)
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

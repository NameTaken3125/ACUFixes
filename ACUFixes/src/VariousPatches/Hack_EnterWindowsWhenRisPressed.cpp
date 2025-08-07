#include "pch.h"

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"
#include "Hack_EnterWindowsWhenRisPressed.h"

#include "ACU/Parkour_PotentialWindowEntry.h"
#include "ACU/SmallArray.h"
#include "ACU/Parkour_WhileOnWallSystem.h"
#include "Common_Plugins/ACU_InputUtils.h"

#include "MainConfig.h"

uint32_t float_bytes(float f) { return (uint32_t&)f; }
void Patch_RunWindowEntryTesterIfRequested_cppTrampoline(AssemblerContext* m_ctx);
void Patch_AlwaysRunWindowEntryTester(AssemblerContext* m_ctx);
void Patch_FakeIsMovingFlag(AssemblerContext* m_ctx);
void WhenCheckingIfWindowIsToBeEnteredNow_ConfirmIfRequestedForThisFrame(AllRegisters* params)
{
    if (ACU::Input::IsPressed(g_Config.hacks->enterWindowsByPressingAButton->enterWindowsButton))
    {
        *(uint32*)(params->rcx_ + 0xDC) = 6;
    }
}
#include "ACU_DefineNativeFunction.h"
static bool IsEnabled_AssistedEnterClosets()
{
    return g_Config.hacks->enterWindowsByPressingAButton->alsoEnterNearbyHidespotClosets;
}
DEFINE_GAME_FUNCTION(IsShouldEnterHideyClosetNow, 0x14265B350, char, __fastcall, (__int64 a1, __int64 a2));
void WhenCheckingIfHideyClosetIsToBeEnteredNow_ConfirmIfRequestedForThisFrame(AllRegisters* params)
{
    if (IsEnabled_AssistedEnterClosets())
    {
        if (ACU::Input::IsPressed(g_Config.hacks->enterWindowsByPressingAButton->enterWindowsButton))
        {
            params->GetRAX() = 1;
            return;
        }
    }
    params->GetRAX() = IsShouldEnterHideyClosetNow(params->rcx_, params->rdx_);
}
void WhenCheckingIfMovementMagnitudeIsNonzeroAndRunningWindowEntryScanner_ForceRunScansIfHotkeyIsPressed(AllRegisters* params);
void WhenCheckingIfPlayerIsMovingBeforePerformingSortAndSelect_ForceRunIfHotkeyIsPressed(AllRegisters* params);
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
    uintptr_t whenCheckingIfWindowIsToBeEnteredNow = 0x1401B0790;
    PresetScript_CCodeInTheMiddle(whenCheckingIfWindowIsToBeEnteredNow, 7,
        WhenCheckingIfWindowIsToBeEnteredNow_ConfirmIfRequestedForThisFrame, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenCheckingIfHideyClosetIsToBeEnteredNow = 0x14265A2DF;
    PresetScript_CCodeInTheMiddle(whenCheckingIfHideyClosetIsToBeEnteredNow, 5,
        WhenCheckingIfHideyClosetIsToBeEnteredNow_ConfirmIfRequestedForThisFrame, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);

    {
        const uintptr_t beforeStartedWindowEntryScan = 0x141A4C60F;
        const uintptr_t afterFinishedWindowEntryScan = 0x141A4C69B;
        PresetScript_CCodeInTheMiddle(beforeStartedWindowEntryScan, 9,
            WhenCheckingIfMovementMagnitudeIsNonzeroAndRunningWindowEntryScanner_ForceRunScansIfHotkeyIsPressed, afterFinishedWindowEntryScan, false);
        PresetScript_CCodeInTheMiddle(0x141A4CBDC, 7,
            WhenCheckingIfPlayerIsMovingBeforePerformingSortAndSelect_ForceRunIfHotkeyIsPressed, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    }
}
Vector2f RotateVector45DegreesLeft(Vector2f vec)
{
    constexpr float cos45deg = 0.70710678118654f;
    return Vector2f{ vec.x - vec.y, vec.x + vec.y } * cos45deg;
}

using AlignedVec4 = __declspec(align(16)) Vector4f;
DEFINE_GAME_FUNCTION(WindowEntryTester__InitializeForFrame_mb, 0x1401858D0, int, __fastcall, (ParkourTester_WindowEntry* a1, Vector4f* p_handsPosition, Vector4f* p_movementDirInGroundPlane, float p_WASDmagnitude, int p_eq6fullLean, __int64 p_currentLedge_mb));
DEFINE_GAME_FUNCTION(FindMatchingParkourActionsForCurrentMovement_P, 0x140185630, __int64, __fastcall, (ParkourTester_WindowEntry* a1, Vector4f* p_handsPosition, Vector4f* p_towardWallCoplanarWithPlayerEntity, Vector4f* p_movementDirInGroundPlane, float p_WASDmagnitude, int p_eq6_fullLean, unsigned __int8 a7, __int64 p_currentLedge_mb, SmallArray<PotentialWindowEntry*>* p_arrayPotentialMovesOut));
static void RunWindowEntryTesterInitAndScanForDirection(
    ParkourTester_WindowEntry& windowEntryTester
    , Vector4f& handsPosition
    , Vector4f& direction
    , float movementMagnitude
    , int p_eq6fullLean
    , __int64 p_currentLedge_mb
    , Vector4f& p_towardWallCoplanarWithPlayerEntity
    , unsigned __int8 a7
    , SmallArray<PotentialWindowEntry*>& p_arrayPotentialMovesOut
)
{
    WindowEntryTester__InitializeForFrame_mb(&windowEntryTester, &handsPosition,
        &direction, movementMagnitude, p_eq6fullLean, p_currentLedge_mb);
    FindMatchingParkourActionsForCurrentMovement_P(&windowEntryTester, &handsPosition, &p_towardWallCoplanarWithPlayerEntity,
        &direction, movementMagnitude, p_eq6fullLean, a7, p_currentLedge_mb, &p_arrayPotentialMovesOut);
}
void WhenCheckingIfMovementMagnitudeIsNonzeroAndRunningWindowEntryScanner_ForceRunScansIfHotkeyIsPressed(AllRegisters* params)
{
    auto& whileOnWall = *(WhileOnWallSystem*)params->r15_;

    auto& windowEntryTester = *whileOnWall.windowEntryTester;
    Vector4f* handsPosition = (Vector4f*)(params->rbp_ - 0x60);
    Vector4f* originalDirection = (Vector4f*)(params->rbp_ - 0x80);
    Vector4f* towardWallCoplanarWithPlayerEntity = (Vector4f*)(params->rbp_ - 0x30);
    float originalMovementMagnitude = params->XMM7.f0;
    int p_eq6fullLean = (int&)params->rdi_;
    byte a7 = (byte&)params->r12_;
    auto& arrayPotentialMovesOut = *(SmallArray<PotentialWindowEntry*>*)(params->rbp_ - 0x70);

    __int64 currentLedge_mb = (__int64)whileOnWall.p28->p0->currentLedge_mb;

    const bool isRequestedEnterWindow = ACU::Input::IsPressed(g_Config.hacks->enterWindowsByPressingAButton->enterWindowsButton);
    if (!isRequestedEnterWindow)
    {
        // Standard execution
        const bool isMovementMagnitudeNonzero = originalMovementMagnitude != 0.0f;
        const bool isShouldRunDefaultScan = isMovementMagnitudeNonzero;
        if (isShouldRunDefaultScan)
        {
            AlignedVec4 direction = *originalDirection;
            float movementMagnitude = originalMovementMagnitude;
            RunWindowEntryTesterInitAndScanForDirection(windowEntryTester, *handsPosition,
                direction, movementMagnitude, p_eq6fullLean, currentLedge_mb, *towardWallCoplanarWithPlayerEntity, a7, arrayPotentialMovesOut);
        }
        return;
    }

    // Requested to enter window if one is nearby.

    Vector2f playerForward = *(Vector2f*)towardWallCoplanarWithPlayerEntity;
    playerForward.normalize();
    Vector2f diagonalForwardLeft = RotateVector45DegreesLeft(playerForward);
    // The sensor often prioritizes windows that are located below player, sometimes _way_ below,
    // even though there may be a different window very close nearby (e.g. Arno is holding
    // onto a window corner above and to the left, but the sensor would choose to enter a different window
    // 3 meters below).
    // Here, I attempt to stifle this hypersensitivity by first scanning all directions except below,
    // and only then, if nothing's found, scanning the directions below.
    std::array<AlignedVec4, 5> directionsToScanExceptBelow = {
        Vector4f{playerForward.x, playerForward.y, 0, 0},                   // Forward
        Vector4f{playerForward.y, -playerForward.x, 0, 0},                  // Right
        Vector4f{-playerForward.y, playerForward.x, 0, 0},                  // Left
        // See 2nd story window in Franciade at [78.65 87.84 4.77] and at [64.77 71.73 4.76]:
        // The four cardinal directions sadly aren't enough.
        Vector4f{diagonalForwardLeft.x, diagonalForwardLeft.y, 0, 0},       // Forward-left
        Vector4f{diagonalForwardLeft.y, -diagonalForwardLeft.x, 0, 0},      // Forward-right
    };
    std::array<AlignedVec4, 3> directionsBelow = {
        Vector4f{-playerForward.x, -playerForward.y, 0, 0},                 // Below
        Vector4f{-diagonalForwardLeft.x, -diagonalForwardLeft.y, 0, 0},     // Below-right
        Vector4f{-diagonalForwardLeft.y, diagonalForwardLeft.x, 0, 0},      // Below-left
    };
    float fakeMovementMagnitude = 1;
    for (Vector4f& direction : directionsToScanExceptBelow)
    {
        {
            RunWindowEntryTesterInitAndScanForDirection(windowEntryTester, *handsPosition,
                direction, fakeMovementMagnitude, p_eq6fullLean, currentLedge_mb, *towardWallCoplanarWithPlayerEntity, a7, arrayPotentialMovesOut);
        }
    }
    if (!arrayPotentialMovesOut.size)
    {
        for (Vector4f& direction : directionsBelow)
        {
            RunWindowEntryTesterInitAndScanForDirection(windowEntryTester, *handsPosition,
                direction, fakeMovementMagnitude, p_eq6fullLean, currentLedge_mb, *towardWallCoplanarWithPlayerEntity, a7, arrayPotentialMovesOut);
        }
    }
}
void WhenCheckingIfPlayerIsMovingBeforePerformingSortAndSelect_ForceRunIfHotkeyIsPressed(AllRegisters* params)
{
    bool& isMovingFlag = *(bool*)(params->rbp_ + 0xE0);
    const bool isRequestedEnterWindow = ACU::Input::IsPressed(g_Config.hacks->enterWindowsByPressingAButton->enterWindowsButton);
    if (isRequestedEnterWindow)
        isMovingFlag = true;
}

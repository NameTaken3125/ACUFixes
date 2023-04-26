#include "pch.h"

#include "Hack_ReworkedTakeCover.h"

#include "ACU/InputContainer.h"

#include "ACU_DefineNativeFunction.h"

bool IsTakeCoverPressed()
{
    InputContainer& inpCont = InputContainer::GetMainSingleton();
    return inpCont.keyStates_thisFrame.IsPressed(ActionKeyCode::ParkourUp);
}

DEFINE_GAME_FUNCTION(WhenTryToTakeCover_IsSpacePressed_mb, 0x1426689A0, char, __fastcall, (__int64 a1, int a2, char a3, char a4));
void WhenCheckingIfNeedToTakeCover_ForceIfSpacebarIsPressed(AllRegisters* params)
{
    const bool shouldSeekCover = IsTakeCoverPressed();
    if (shouldSeekCover)
    {
        *params->rax_ = 1;
        return;
    }
    *params->rax_ = WhenTryToTakeCover_IsSpacePressed_mb(params->rcx_, params->rdx_, params->r8_, params->r9_);
}
static bool ShouldIForceLeaveCoverWhenLeaningAroundTheCorner()
{
    return true;
}
void WhenLeaningAroundTheCornerAndCheckingIfSprintingOutOfTheCover_ForceLeave(AllRegisters* params)
{
    params->r9_ = params->r12_;
    const bool needToLeaveCover = ShouldIForceLeaveCoverWhenLeaningAroundTheCorner();
    if (needToLeaveCover)
    {
        params->r12_ = true;
    }
}
void WhenLeavingTheCoverBySprintAndForcingSprintStart_DontSprintUnnecessarily(AllRegisters* params)
{
    const bool isReallySprinting = InputContainer::GetMainSingleton().keyStates_thisFrame.IsPressed(ActionKeyCode::Sprint);
    (bool&)params->r9_ = isReallySprinting;
}
ReworkedTakeCover::ReworkedTakeCover()
{
    uintptr_t whenCheckingIfNeedToTakeCover = 0x1426521C6;
    PresetScript_CCodeInTheMiddle(
        whenCheckingIfNeedToTakeCover, 5,
        WhenCheckingIfNeedToTakeCover_ForceIfSpacebarIsPressed, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);

    // There are multiple ways the player can leave cover: press Space, start sprinting
    // or just walk directly away.
    // Normally, if you try to move directly away from the cover without sprinting,
    // you need to hold the direction button for 0.15 seconds
    // before actually leaving. Here, I remove the timer.
    DEFINE_ADDR(whenMovingAwayFromCoverAndCheckingIfTimerForKeypressIsFinished, 0x14192541F);
    DEFINE_ADDR(whenMovingAwayFromCoverAndKeypressTimerIsFinished, 0x141925455);
    whenMovingAwayFromCoverAndCheckingIfTimerForKeypressIsFinished = {
        0xE9, RIP(whenMovingAwayFromCoverAndKeypressTimerIsFinished, 4),
        nop(2)
    };
    auto AlsoAllowDetachIfMovingAwayFromOpenCornerTowardWallContinuation = [&]()
    {
        // If Arno is behind cover, moving away from the open corner toward wall continuation,
        // but is not allowed to move further along,
        // he stops and turns his head away from the corner.
        // Instead of letting him do this, pretend he is moving away from cover.
        // Combined with removed timer, this means Arno immediately unsticks instead of turning his head.
        DEFINE_ADDR(whenBehindCoverTryingToMove_decideIfAlongOrAway, 0x1419253DB);
        DEFINE_ADDR(whenBehindCoverTryingToMove_decideIfAlongOrAway__return, 0x1419253DB + 7);
        DEFINE_ADDR(whenMovingAwayFromCoverInsteadOfAlong, 0x141925412);
        ALLOC(whenBehindCoverTryingToMove_decideIfAlongOrAway__cave, 0x80, 0x1419253DB);

        whenBehindCoverTryingToMove_decideIfAlongOrAway = {
            0xE9, RIP(whenBehindCoverTryingToMove_decideIfAlongOrAway__cave),
            nop(2)
        };
        whenBehindCoverTryingToMove_decideIfAlongOrAway__cave = {
            "48 8B 4B 28"                                                        // - mov rcx,[rbx+28]
            "48 8B 09"                                                           // - mov rcx,[rcx]
            "80 B9 04020000 01"                                                  // - cmp byte ptr [rcx+00000204],01
            "0F84", RIP(whenMovingAwayFromCoverInsteadOfAlong),                  // - je ACU.exe+1925412
            "0FC6 C1 11"                                                         // - shufps xmm0,xmm1,11
            "0F58 C1"                                                            // - addps xmm0,xmm1
            "E9", RIP(whenBehindCoverTryingToMove_decideIfAlongOrAway__return)   // - jmp ACU.exe+19253E2
        };
    };
    AlsoAllowDetachIfMovingAwayFromOpenCornerTowardWallContinuation();

    uintptr_t whenLeaningAroundTheCornerAndCheckingIfSprintingOutOfTheCover = 0x142656652;
    PresetScript_CCodeInTheMiddle(
        whenLeaningAroundTheCornerAndCheckingIfSprintingOutOfTheCover, 8,
        WhenLeaningAroundTheCornerAndCheckingIfSprintingOutOfTheCover_ForceLeave, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    uintptr_t whenLeavingTheCoverBySprintAndForcingSprintStart = 0x1419144F1;
    PresetScript_CCodeInTheMiddle(
        whenLeavingTheCoverBySprintAndForcingSprintStart, 5,
        WhenLeavingTheCoverBySprintAndForcingSprintStart_DontSprintUnnecessarily, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    // Disable `movzx r9d,r12l` that was instead executed in the above hook.
    uintptr_t whenBehindCoverAndDisallowingBombAimIfSprintIsPressed = 0x14265665C;
    PresetScript_NOP(
        whenBehindCoverAndDisallowingBombAimIfSprintIsPressed, 4);
}

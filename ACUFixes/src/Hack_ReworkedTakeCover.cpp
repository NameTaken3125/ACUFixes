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

#include "ACU/HumanStatesHolder.h"
DEFINE_GAME_FUNCTION(createsLeaveCoverFunctor, 0x141979050, char, __fastcall, (FunctorBase* p_node, __m128* p_directionInGroundPlane, float p_wasdMagnitude, char p_doRunOut));
char* g_WhenBehindCover_flag_IsStartedDetaching = nullptr;
void WhenBehindCoverFunctionPrologue_SaveParams(AllRegisters* params)
{
    g_WhenBehindCover_flag_IsStartedDetaching = *(char**)(params->GetRSP() + 0x28);
}
#include "ACU/Entity.h"

template<typename T>
T& ResolveMultipleOffsets(uintptr_t start, const std::vector<int>& offsets, int addAtTheEnd = 0)
{
    uintptr_t curr = start;
    for (int offset : offsets)
    {
        curr = *(uintptr_t*)(curr + offset);
    }
    return *(T*)(curr + addAtTheEnd);
}
void WhenBehindCoverStandingStillOrMovingAlong_DetachIfTryingToMoveButStuckNowhereToGo(AllRegisters* params)
{
    const bool isTryingToMove = params->rdx_ & 0xFF;
    const float detectedSizeOfWalkableSpaceInTheDirectionOfCurrentMovement_mb = *(float*)(params->GetRSP() + 8 * 7);
    bool shouldDetach =
        isTryingToMove
        && detectedSizeOfWalkableSpaceInTheDirectionOfCurrentMovement_mb <= 0.1f;
    if (!shouldDetach)
        return;

    __m128* attemptedMovementDirection = *(__m128**)(params->GetRSP() + 8 * 5);
    FunctorBase* inCoverFnct = (FunctorBase*)params->rbx_;

    // Don't detach until the player has turned to face the direction of "stuckness".
    // Example: [76.92 -279.66 0.50]
    {
        HumanStatesHolder* humanStates = (HumanStatesHolder*)inCoverFnct->parentStack[2];
        Entity* player = humanStates->player;
        const bool isFacingTheDirectionOfAttemptedMovement =
            ((Vector3f*)attemptedMovementDirection)
            ->dotProduct(player->GetDirectionForward()) > 0;
        if (!isFacingTheDirectionOfAttemptedMovement)
        {
            shouldDetach = false;
        }
    }
    if (g_WhenBehindCover_flag_IsStartedDetaching && *g_WhenBehindCover_flag_IsStartedDetaching)
        shouldDetach = false;

    if (!shouldDetach)
        return;

    auto node = inCoverFnct->parentStack[1];
    __m128* leaveCoverDirection = attemptedMovementDirection;
    createsLeaveCoverFunctor(node, leaveCoverDirection, 1.0f, 0);
    if (g_WhenBehindCover_flag_IsStartedDetaching)
    {
        *g_WhenBehindCover_flag_IsStartedDetaching = 1;
        g_WhenBehindCover_flag_IsStartedDetaching = nullptr;
    }
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

    auto AlsoDetachWhenMovingAlongTheWallAndSuddenlyAreStuck = [&]()
    {
        // When behind cover, there are some spots where Arno is moving along the wall,
        // and just stops midwall. Not turning his head, sort of leaning in that direction, but just stops,
        // pretty much fails to go further for no reason. I imagine this has something to do
        // with edges of invisible "cover meshes" or something.
        // The point is he gets stuck, and I'd like for him to instead detach.
        // Example: "It Belongs in the Museum", [66.24 -283.21 0.50]

        // BUG: Even in unmodded game, if you move along the wall, and manually detach by pressing Space,
        // then immediately start moving in the opposite direction, Arno will do
        // several quick weird direction changes. Since this hack will make you detach automatically
        // in places where you should really just be able to keep hugging the wall,
        // I worry this detaching will be sudden enough to provoke instictive change of direction,
        // which might produce the aforementioned direction flips, which might be even more jarring
        // than getting stuck. TBD.
        uintptr_t whenBehindCoverStandingStillOrMovingAlong = 0x1419261C4;
        PresetScript_CCodeInTheMiddle(
            whenBehindCoverStandingStillOrMovingAlong, 6,
            WhenBehindCoverStandingStillOrMovingAlong_DetachIfTryingToMoveButStuckNowhereToGo, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
        uintptr_t whenBehindCover_functionPrologue = 0x141924BC0;
        PresetScript_CCodeInTheMiddle(
            whenBehindCover_functionPrologue, 7,
            WhenBehindCoverFunctionPrologue_SaveParams, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };
    AlsoDetachWhenMovingAlongTheWallAndSuddenlyAreStuck();
}

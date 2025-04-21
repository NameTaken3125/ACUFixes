#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

/*
- `UsedDuringCrouch` [[[[[[[[[[[[14521AAD0]+40]]+10]+60]+218]+0]+c8]+710]+2a8]+28]+0] has two bool flags for indicating
  if player should be crouched or uncrouched when standing still: 1d0(==`isSupposedToBeCrouched`, primary flag), 1d4(==saywhatflag).
- Player starts off uncrouched, then:
- (A1) If you STAND STILL and press crouch:
  - isSupposedToBeCrouched = 1, saywhatflag = 1
  - the result is that the primary flag is 1, so the player crouches.
- (B1) If you SPRINT and press crouch, no matter how many times you press it:
  - isSupposedToBeCrouched = 1, saywhatflag = 0 (!!!)
  - the result is primary flag is again "1" so that when you stop, the player crouches
  - again, no matter how many times crouch is pressed during a sprint, when you stop, you WILL be crouched
- Now the player is crouched. What happens when you press Crouch/Uncrouch again
  depends on how you got here: case (A1) or (B1).
- (A2):
  - isSupposedToBeCrouched = 1, saywhatflag = 1 ---change into---> isSupposedToBeCrouched = 0, saywhatflag = 0
  - the result is the primary flag is 0, and the player uncrouches.
- (B2):
  - isSupposedToBeCrouched = 1, saywhatflag = 0 ---change into---> isSupposedToBeCrouched = 1, saywhatflag = 1
  - the result is the same as at the end of A1!
  - even though you _are_ crouched, and press Uncrouch, you _stay_ crouched! until you press Uncrouch AGAIN
- now you know why it's called a `saywhatflag`.
- furthermore, if isSupposedToBeCrouched = 1, saywhatflag = 1 (crouched) and you are sprinting, then pressing crouch will immediately
  reset to Uncrouched i.e. the starting-off state, i.e. pressing crouch _again_ will _again_ put you into crouch that can't
  be cancelled until stopped sprinting (with two Crouch taps, _again_!).
- And the fix consists of noping 2 bytes.
  - The result is that regardless of whether the player is sprinting or not, the Crouch/Uncrouch state
    toggles every time Crouch Button is pressed, so pressing it an even number of times leaves you in the same state.
    - Unless the game can lose some "JustPressedButton" events, which I'm not sure doesn't happen.
*/
struct CrouchFix : AutoAssemblerCodeHolder_Base
{
    CrouchFix()
    {
        uintptr_t whenTryingToCrouch_dontEnterCrouchModeUntilStoppedSprinting = 0x141AFBE56;
        PresetScript_NOP(
            whenTryingToCrouch_dontEnterCrouchModeUntilStoppedSprinting, 2);
        // Yep, that's a two-byte fix.
    }
};

#include "pch.h"
#include "Hack_NoMoreFailedBombThrows.h"

#include "ACU/HumanStatesHolder.h"
#include "ACU_DefineNativeFunction.h"

DEFINE_GAME_FUNCTION(HumanStatesHolder__OnThrowBomb_P, 0x141988870, void, __fastcall, (HumanStatesHolder* a1, __m128* a2, char a3));
bool WhenAimAndThrowBombAnimationExits_IsShouldForceSpawnBombNow(HumanStatesHolder& humanStates)
{
    auto hasAnimationEventsData_mb = humanStates.hasAnimationEventsData_mb;
    if (!hasAnimationEventsData_mb)
    {
        return false;
    }
    constexpr int idx_BombThrowAnimationEvent_mb = 24;
    auto& dataOfBombThrowAnim = hasAnimationEventsData_mb->animEvents_mb[idx_BombThrowAnimationEvent_mb];
    const bool isThrowAnimationPlayingButNotYetFinished = dataOfBombThrowAnim.dword_10_isPlaying_mb == 1;
    if (isThrowAnimationPlayingButNotYetFinished)
    {
        return true;
    }
    return false;
}
void WhenAimAndThrowBombAnimationExits_ForceThrowBombIfThrowAnimationWasInterrupted(HumanStatesHolder& humanStates)
{
    const bool shouldThrowBomb = WhenAimAndThrowBombAnimationExits_IsShouldForceSpawnBombNow(humanStates);
    if (!shouldThrowBomb) { return; }

    __m128 a2;
    std::memset(&a2, 0, sizeof(a2));
    HumanStatesHolder__OnThrowBomb_P(&humanStates, &a2, 1);
}
void FunctorAimAndThrowBombExit_fromGround_ThrowBombIfSupposedToAndNotAlreadyThrown(AllRegisters* params)
{
    HumanStatesHolder* humanStates = *(HumanStatesHolder**)(*(uint64*)(params->rcx_ + 0x28) + 0x28);
    if (!humanStates) { return; }
    WhenAimAndThrowBombAnimationExits_ForceThrowBombIfThrowAnimationWasInterrupted(*humanStates);
}
void FunctorAimAndThrowBombExit_fromLedge_ThrowBombIfSupposedToAndNotAlreadyThrown(AllRegisters* params)
{
    HumanStatesHolder* humanStates = *(HumanStatesHolder**)(*(uint64*)(params->rcx_ + 0x28) + 0x30);
    if (!humanStates) { return; }
    WhenAimAndThrowBombAnimationExits_ForceThrowBombIfThrowAnimationWasInterrupted(*humanStates);
}
void FunctorAimAndThrowBombExit_fromCover_ThrowBombIfSupposedToAndNotAlreadyThrown(AllRegisters* params)
{
    HumanStatesHolder* humanStates = *(HumanStatesHolder**)(*(uint64*)(params->rcx_ + 0x28) + 0x20);
    if (!humanStates) { return; }
    WhenAimAndThrowBombAnimationExits_ForceThrowBombIfThrowAnimationWasInterrupted(*humanStates);
}
NoMoreFailedBombThrows::NoMoreFailedBombThrows()
{
    uintptr_t Functor_AimAndThrowBomb_fromGround_Exit = 0x141A795F0;
    uintptr_t Functor_AimAndThrowBomb_fromLedge_Exit = 0x141A5A400;
    uintptr_t Functor_AimAndThrowBomb_fromCover_Exit = 0x1419629C0;
    PresetScript_CCodeInTheMiddle(Functor_AimAndThrowBomb_fromGround_Exit, 5,
        FunctorAimAndThrowBombExit_fromGround_ThrowBombIfSupposedToAndNotAlreadyThrown, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    PresetScript_CCodeInTheMiddle(Functor_AimAndThrowBomb_fromLedge_Exit, 6,
        FunctorAimAndThrowBombExit_fromLedge_ThrowBombIfSupposedToAndNotAlreadyThrown, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    PresetScript_CCodeInTheMiddle(Functor_AimAndThrowBomb_fromCover_Exit, 8,
        FunctorAimAndThrowBombExit_fromCover_ThrowBombIfSupposedToAndNotAlreadyThrown, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

#include "pch.h"

#include "ACU/Entity.h"
#include "ACU/HumanStatesHolder.h"
#include "ACU/SharedPtr.h"

#include "Hack_DontForceUnsheatheWhenInDisguise.h"



Entity* GetCurrentDisguiseTargetEntity()
{
    auto* humanStates = HumanStatesHolder::GetForPlayer(); if (!humanStates) { return nullptr; }
    auto* _d0 = humanStates->humanStatesHolder_D0; if (!_d0) { return nullptr; }
    auto* usedDuringDisguise = _d0->usedDuringDisguise; if (!usedDuringDisguise) { return nullptr; }
    auto* disguiseTargetShared = usedDuringDisguise->disguiseTargetEntity; if (!disguiseTargetShared) { return nullptr; }
    Entity* disguiseTarget = disguiseTargetShared->GetPtr();
    return disguiseTarget;
}
bool IsPlayerInDisguise()
{
    auto* humanStates = HumanStatesHolder::GetForPlayer(); if (!humanStates) { return false; }
    auto* _d0 = humanStates->humanStatesHolder_D0; if (!_d0) { return false; }
    auto* usedDuringDisguise = _d0->usedDuringDisguise; if (!usedDuringDisguise) { return false; }
    return usedDuringDisguise->isInDisguise_mb;
}
std::optional<Vector3f> GetDisguiseTargetPosition()
{
    Entity* disguiseTarget = GetCurrentDisguiseTargetEntity();
    if (!disguiseTarget) {
        return {};
    }
    return disguiseTarget->GetPosition();
}
#include "ACU_DefineNativeFunction.h"
DEFINE_GAME_FUNCTION(onEnterFight_canDisableUnsheathing_P, 0x1426582C0, __int64, __fastcall, (__int64 a1, __int64 a2, char a3));

bool IsPlayerInQuickshot()
{
    auto* humanStates = HumanStatesHolder::GetForPlayer();
    if (!humanStates) { return false; }
    auto* callbacksThatCheckIfQuickshotIsActive = humanStates->callbacksForIdx[55];
    if (!callbacksThatCheckIfQuickshotIsActive) {
        return false;
    }
    return callbacksThatCheckIfQuickshotIsActive->callbacksWithReceivers.size > 0;
}
#include "ACU/VanishingManager.h"
static bool IsPlayerCrowdBlending()
{
    auto* vanishMan = VanishingManager::GetSingleton();
    const bool isCrowdBlending = vanishMan->_8permanentBlend2crowdBlend0x40moneyPouch == 2;
    return isCrowdBlending;
}
#include "ACU/Clock.h"
#include "ACU/World.h"
constexpr float durationAllowAssassination_whenCombatJustStarted = 0.7f;
float g_lastTimePlayerWasDetected = 0;
Clock* g_lastTimePlayerWasDetected_clock = nullptr;
Clock* ACT_GetCurrentClock()
{
    auto* world = World::GetSingleton();
    if (!world) { return nullptr; }
    return &world->clockInWorldWithSlowmotion;
}
static bool IsCoyoteTimeForAssassination()
{
    Clock* currentClock = ACT_GetCurrentClock();
    if (!(currentClock && currentClock == g_lastTimePlayerWasDetected_clock)) { return false; }
    const float currentTime = currentClock->GetCurrentTimeFloat();
    return g_lastTimePlayerWasDetected <= currentTime && currentTime < g_lastTimePlayerWasDetected + durationAllowAssassination_whenCombatJustStarted;
}
void WhenPlaySoundConflictStart_RememberTime(AllRegisters* params)
{
    g_lastTimePlayerWasDetected_clock = ACT_GetCurrentClock();
    if (g_lastTimePlayerWasDetected_clock)
    {
        g_lastTimePlayerWasDetected = g_lastTimePlayerWasDetected_clock->GetCurrentTimeFloat();
    }
}
static bool IsShouldNOTAutomaticallyUnsheathe()
{
    return IsPlayerInDisguise()
        || IsPlayerCrowdBlending()
        || IsPlayerInQuickshot()
        || IsCoyoteTimeForAssassination()
        ;
}
static bool IsShouldForceAllowAssassination()
{
     return IsShouldNOTAutomaticallyUnsheathe();
}
void WhenInCloseRangeDontForceUnsheathe(AllRegisters* params)
{
    const bool shouldSkipUnsheathing = IsShouldNOTAutomaticallyUnsheathe();
    if (shouldSkipUnsheathing)
    {
        *params->rax_ = 0;
        return;
    }
    *params->rax_ = onEnterFight_canDisableUnsheathing_P(params->rbp_, params->rsi_, params->r8_);
}
class SharedPtr_mb;
DEFINE_GAME_FUNCTION(WhenDecidingIfAssassinationShouldBeDisallowed_Stage1ChaseVersion, 0x1404E90C0, char, __fastcall, (__int64 a1, SharedPtr_mb* a2, SharedPtr_mb* a3));
DEFINE_GAME_FUNCTION(WhenDecidingIfAssassinationShouldBeDisallowed_Stage2ChaseVersion, 0x1404E9310, char, __fastcall, (__int64 a1, SharedPtr_mb* a2, SharedPtr_mb* a3));
void WhenDecidingIfAssassinationShouldBeDisallowed_Stage1ChaseVersion_ForceAllowAssassination(AllRegisters* params)
{
    if (IsShouldForceAllowAssassination())
    {
        *params->rax_ = 0;
        return;
    }
    *params->rax_ = WhenDecidingIfAssassinationShouldBeDisallowed_Stage1ChaseVersion(params->rcx_, (SharedPtr_mb*)(params->rdx_), (SharedPtr_mb*)(params->r8_));
}
void WhenDecidingIfAssassinationShouldBeDisallowed_Stage2ChaseVersion_ForceAllowAssassination(AllRegisters* params)
{
    if (IsShouldForceAllowAssassination())
    {
        *params->rax_ = 0;
        return;
    }
    *params->rax_ = WhenDecidingIfAssassinationShouldBeDisallowed_Stage2ChaseVersion(params->rcx_, (SharedPtr_mb*)(params->rdx_), (SharedPtr_mb*)(params->r8_));
}
DontUnsheatheLikeAnIdiotWhileInDisguise::DontUnsheatheLikeAnIdiotWhileInDisguise()
{
    const uintptr_t whenCombatIsOngoingAndPlayerGetsCloseEnoughToUnsheathe = 0x14265D16B;
    const uintptr_t returnToAfterTheOriginalCallIsMade = 0x14265D170;
    const bool dontExecuteStolenBytes = false;
    PresetScript_CCodeInTheMiddle(
        whenCombatIsOngoingAndPlayerGetsCloseEnoughToUnsheathe, 5
        , WhenInCloseRangeDontForceUnsheathe, returnToAfterTheOriginalCallIsMade, dontExecuteStolenBytes);

    const uintptr_t whenDecidingIfAssassinationsShouldBeDisallowed_stage1chase = 0x140CD9BED;
    PresetScript_CCodeInTheMiddle(
        whenDecidingIfAssassinationsShouldBeDisallowed_stage1chase, 5
        , WhenDecidingIfAssassinationShouldBeDisallowed_Stage1ChaseVersion_ForceAllowAssassination, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    const uintptr_t whenDecidingIfAssassinationsShouldBeDisallowed_stage2chase = 0x140CD9C95;
    PresetScript_CCodeInTheMiddle(
        whenDecidingIfAssassinationsShouldBeDisallowed_stage2chase, 5
        , WhenDecidingIfAssassinationShouldBeDisallowed_Stage2ChaseVersion_ForceAllowAssassination, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);

    const uintptr_t whenPlaySoundConflictStart = 0x1410CEB60;
    PresetScript_CCodeInTheMiddle(
        whenPlaySoundConflictStart, 7,
        WhenPlaySoundConflictStart_RememberTime, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

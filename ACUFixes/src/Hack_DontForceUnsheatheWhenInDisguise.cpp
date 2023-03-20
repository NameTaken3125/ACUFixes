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
    SharedPtr_mb* disguiseTargetShared = usedDuringDisguise->disguiseTargetEntity; if (!disguiseTargetShared) { return nullptr; }
    Entity* disguiseTarget = disguiseTargetShared->entity_mb;
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

void WhenInCloseRangeDontForceUnsheathe(AllRegisters* params)
{
    bool shouldSkipUnsheathing = true;
    shouldSkipUnsheathing = IsPlayerInDisguise();
    if (shouldSkipUnsheathing)
    {
        *params->rax_ = 0;
        return;
    }
    *params->rax_ = onEnterFight_canDisableUnsheathing_P(params->rbp_, params->rsi_, params->r8_);
}
bool ShouldForceAllowAssassination() {
    return IsPlayerInDisguise();
}
class SharedPtr_mb;
DEFINE_GAME_FUNCTION(WhenDecidingIfAssassinationShouldBeDisallowed_Stage1ChaseVersion, 0x1404E90C0, char, __fastcall, (__int64 a1, SharedPtr_mb* a2, SharedPtr_mb* a3));
DEFINE_GAME_FUNCTION(WhenDecidingIfAssassinationShouldBeDisallowed_Stage2ChaseVersion, 0x1404E9310, char, __fastcall, (__int64 a1, SharedPtr_mb* a2, SharedPtr_mb* a3));
void WhenDecidingIfAssassinationShouldBeDisallowed_Stage1ChaseVersion_ForceAllowAssassination(AllRegisters* params)
{
    if (ShouldForceAllowAssassination())
    {
        *params->rax_ = 0;
        return;
    }
    *params->rax_ = WhenDecidingIfAssassinationShouldBeDisallowed_Stage1ChaseVersion(params->rcx_, (SharedPtr_mb*)(params->rdx_), (SharedPtr_mb*)(params->r8_));
}
void WhenDecidingIfAssassinationShouldBeDisallowed_Stage2ChaseVersion_ForceAllowAssassination(AllRegisters* params)
{
    if (ShouldForceAllowAssassination())
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
}

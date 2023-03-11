#include "pch.h"

#include "ACU/AtomAnimComponent.h"
#include "ACU/Human.h"
#include "ACU/Entity.h"
#include "ACU/HasBallisticAimingEquipmentType.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU/SharedPtr.h"

#include "Hack_DontForceUnsheatheWhenInDisguise.h"



class UsedDuringDisguise
{
public:
    char pad_0000[576]; //0x0000
    SharedPtr_mb* disguiseTargetEntity; //0x0240
    char pad_0248[48]; //0x0248
    uint8 isInDisguise_mb; //0x0278
    char pad_0279[15]; //0x0279
};
assert_offsetof(UsedDuringDisguise, disguiseTargetEntity, 0x240);
class HumanStatesHolder_D0
{
public:
    UsedDuringDisguise* usedDuringDisguise; //0x0000
    char pad_0008[256]; //0x0008
}; //Size: 0x0108
class HumanStatesHolder_mb
{
public:
    char pad_0000[208]; //0x0000
    HumanStatesHolder_D0* humanStatesHolder_D0; //0x00D0
    char pad_00D8[3164]; //0x00D8
    uint32 ballisticAimingCurrentEquipmentType; //0x0D34
    char pad_0D38[568]; //0x0D38
    BallisticProjectileAimingProcess aimingMoney; //0x0F70
    BallisticProjectileAimingProcess aiming_equipType0x12_1170; //0x1170
    BallisticProjectileAimingProcess aimingPoison; //0x1370
    BallisticProjectileAimingProcess aimingDefault; //0x1570
    BallisticProjectileAimingProcess aiming_equip19_1770; //0x1770
    BallisticProjectileAimingProcess aimingGuillotineGun; //0x1970
    char pad_1B70[816]; //0x1B70
}; //Size: 0x1EA0
static AtomAnimComponent* GetPlayerAtomAnimComponent()
{
    Entity* player = ACU::GetPlayer();
    if (!player) { return nullptr; }
    int cpntIdx_atomAnimCpnt = player->cpntIndices_157.atomAnimCpnt;
    return static_cast<AtomAnimComponent*>(player->cpnts_mb[cpntIdx_atomAnimCpnt]);
}
Entity* GetCurrentDisguiseTargetEntity()
{
    AtomAnimComponent* atomAnimCpnt = GetPlayerAtomAnimComponent();
    if (!atomAnimCpnt) { return nullptr; }
    auto* human = atomAnimCpnt->human_c58; if (!human) { return nullptr; }
    auto* humanStates = (HumanStatesHolder_mb*)human->hasBallisticAimingEquipmentType_710; if (!humanStates) { return nullptr; }
    auto* _d0 = humanStates->humanStatesHolder_D0; if (!_d0) { return nullptr; }
    auto* usedDuringDisguise = _d0->usedDuringDisguise; if (!usedDuringDisguise) { return nullptr; }
    SharedPtr_mb* disguiseTargetShared = usedDuringDisguise->disguiseTargetEntity; if (!disguiseTargetShared) { return nullptr; }
    Entity* disguiseTarget = disguiseTargetShared->entity_mb;
    return disguiseTarget;
}
bool IsPlayerInDisguise()
{
    AtomAnimComponent* atomAnimCpnt = GetPlayerAtomAnimComponent();
    if (!atomAnimCpnt) { return false; }
    auto* human = atomAnimCpnt->human_c58; if (!human) { return false; }
    auto* humanStates = (HumanStatesHolder_mb*)human->hasBallisticAimingEquipmentType_710; if (!humanStates) { return false; }
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
#define DEFINE_GAME_FUNCTION(FuncName, relativeOffset, returnType, callingConvention, allParamsInParentheses) \
using FuncName##_t = returnType(callingConvention*)allParamsInParentheses;\
FuncName##_t FuncName = (FuncName##_t)relativeOffset;
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

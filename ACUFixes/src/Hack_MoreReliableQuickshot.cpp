#include "pch.h"

#include "Hack_MoreReliableQuickshot.h"
#include "ACU/Entity.h"
#include "ACU/HumanStatesHolder.h"
#include "ACU/CSrvPlayerWeaponSwitch.h"
#include "ACU/ACUGetSingletons.h"

#include "ACU_DefineNativeFunction.h"

#include "MainConfig.h"
bool BetterQuickshot_IsLessRestrictionsEnabled()
{
    return g_Config.hacks->moreReliableQuickshot->lessQuickshotRestrictions;
}

bool IsPlayerParkouringBothHandsBusy();
bool IsCurrentRangedWeaponTwohanded();
static bool BetterQuickshot_IsWaitingForQuickshotToComplete();
static bool BetterQuickshot_IsShouldAllowToProceedWithoutReholstering_Player(int p_1melee2ranged)
{
    bool isQuickshot = BetterQuickshot_IsWaitingForQuickshotToComplete();
    if (!isQuickshot)
    {
        return false;
    }
    bool isOneHandedRanged = p_1melee2ranged == 2;
    if (isOneHandedRanged)
    {
        return true;
    }
    const bool isTwoHandedRanged = IsCurrentRangedWeaponTwohanded();
    const bool isPureMelee = !isTwoHandedRanged;
    if (isPureMelee)
    {
        return false;
    }
    const bool isStillAllowedToQuickshotTwohandedMelee = !IsPlayerParkouringBothHandsBusy();
    return isStillAllowedToQuickshotTwohandedMelee;
}
DEFINE_GAME_FUNCTION(ReattachWeaponToSheathOrHolster, 0x141B05570, void, __fastcall, (__int64 a1, int p_1melee2ranged));
bool WhenInstantSheathingOrReholstering_IsShouldProceedWithoutReholstering(Entity* ownerEntity, int p_1melee2ranged)
{
    const bool isPlayer = ownerEntity && ownerEntity == ACU::GetPlayer();
    if (!isPlayer)
    {
        return false;
    }
    return BetterQuickshot_IsShouldAllowToProceedWithoutReholstering_Player(p_1melee2ranged);
}
void WhenInstantSheathingOrReholsteringDueToParkour_DontDoThat(AllRegisters* params)
{
    if (!BetterQuickshot_IsLessRestrictionsEnabled())
    {
        ReattachWeaponToSheathOrHolster(params->rcx_, params->rdx_);
        return;
    }
    const int p_1melee2ranged = (int&)params->rdx_;
    FunctorBase* node = (FunctorBase*)params->rcx_;
    auto* humanStates = node->GetNthParent<HumanStatesHolder>(0);
    Entity* ownerEntity = humanStates->ownerEntity;
    if (WhenInstantSheathingOrReholstering_IsShouldProceedWithoutReholstering(ownerEntity, p_1melee2ranged))
    {
        return;
    }
    ReattachWeaponToSheathOrHolster(params->rcx_, params->rdx_);
}
void WhenInstantSheathingOrReholsteringDueToAssassinationStart_DontDoThat(AllRegisters* params)
{
    if (!BetterQuickshot_IsLessRestrictionsEnabled())
    {
        ReattachWeaponToSheathOrHolster(params->rcx_, params->rdx_);
        return;
    }
    const int p_1melee2ranged = (int&)params->rdx_;
    FunctorBase* node = (FunctorBase*)params->rcx_;
    auto* humanStates = node->GetNthParent<HumanStatesHolder>(0);
    Entity* ownerEntity = humanStates->ownerEntity;
    if (WhenInstantSheathingOrReholstering_IsShouldProceedWithoutReholstering(ownerEntity, p_1melee2ranged))
    {
        return;
    }
    ReattachWeaponToSheathOrHolster(params->rcx_, params->rdx_);
}


bool IsPlayerParkouringBothHandsBusy()
{
    auto* humanStates = HumanStatesHolder::GetForPlayer();
    if (!humanStates) {
        return false;
    }
    auto* navigationCallbacks = humanStates->callbacksForIdx[366];
    if (!navigationCallbacks) {
        return false;
    }
    if (!navigationCallbacks->callbacksWithReceivers.size) { return false; }
    uint64 WhileOnWallSystem_VTable = 0x142FFC970;
    uint64 WhileInVerticalWallrun_VTable = 0x142FFCAA8;
    uint64 WhileInSwingHang_VTable = 0x142FFCB20;
    uint64 WhileInLeap_VTable = 0x142FFD1E0;
    uint64 currentParkourModeVTable = *(uint64*)(navigationCallbacks->callbacksWithReceivers[0].humanStateNode);
    return currentParkourModeVTable == WhileOnWallSystem_VTable
        || currentParkourModeVTable == WhileInVerticalWallrun_VTable
        || currentParkourModeVTable == WhileInSwingHang_VTable
        //|| currentParkourModeVTable == WhileInLeap_VTable
        ;
}
CSrvPlayerWeaponSwitch* FindPlayerWeaponSwitch();
bool IsCurrentRangedWeaponTwohanded()
{
    CSrvPlayerWeaponSwitch* weaponSwitcher = FindPlayerWeaponSwitch();
    if (weaponSwitcher)
    {
        EquipmentType rangedWeaponType = weaponSwitcher->equipType_LeftHand;
        return rangedWeaponType == EquipmentType::Rifle
            || rangedWeaponType == EquipmentType::GuillotineGun
            || rangedWeaponType == EquipmentType::Blunderbuss
            ;
    }
    return false;
}


//==============================================================================
// Lifetime hooks: determine when Quickshot and Assassination states are active
//==============================================================================

struct Functor_Quickshot : public FunctorBase
{
public:
};
class Functor_Quickshot_up0 : public FunctorBase
{
public:
    char pad_0100[216]; //0x0100
    SharedPtrNew<Entity>* shared_quickshotTarget; //0x01D8
    char pad_01E0[112]; //0x01E0
}; //Size: 0x0250
assert_offsetof(Functor_Quickshot_up0, shared_quickshotTarget, 0x1D8);
struct PlayerQuickshotState
{
    Functor_Quickshot& m_functorQS;
    Entity* m_targetEntity;
    bool m_isShotDischargedYet = false;
    bool m_isSupposedToHaveEndedAlready = false;
    bool m_isAssassinationStartedAfterQuickshotStarted = false;
    bool m_isAssassinationEndedAfterQuickshotStarted = false;
    PlayerQuickshotState(Functor_Quickshot& functorQS)
        : m_functorQS(functorQS)
    {
        m_targetEntity = functorQS.GetNthParent<Functor_Quickshot_up0>(0)->shared_quickshotTarget->GetPtr();
    }
};
struct PlayerAssassinationAttemptState
{
    Entity* targetNPC = nullptr;
    Entity* secondOptionalTargetNPC = nullptr;
};
std::optional<PlayerAssassinationAttemptState> g_PlayerAssassinationAttemptState;
std::optional<PlayerQuickshotState> g_PlayerQuickshot;
Entity* GetChokedEntity()
{
    return g_PlayerAssassinationAttemptState
        ? g_PlayerAssassinationAttemptState->targetNPC
        : nullptr;
}
Entity* GetQuickshotTargetEntity()
{
    return g_PlayerQuickshot
        ? g_PlayerQuickshot->m_targetEntity
        : nullptr;
}
void PlayerHasJustEnteredQuickshot(Functor_Quickshot& functorQS)
{
    g_PlayerQuickshot.emplace(functorQS);
}
DEFINE_GAME_FUNCTION(HumanStatesHolder__canUnglueWristbow, 0x141AFAE60, void, __fastcall, (HumanStatesHolder* a1, Entity* a2, __int64 p_0));
void TryToCleanUpQuickshotAnimationToPreventArmOutstretchedBug()
{
    HumanStatesHolder* humanStates = HumanStatesHolder::GetForPlayer();
    if (!humanStates) { return; }
    HumanStatesHolder__canUnglueWristbow(humanStates, humanStates->ownerEntity, 0);
}
void PlayerHasJustExitedQuickshot()
{
    if (g_PlayerQuickshot->m_isAssassinationEndedAfterQuickshotStarted)
    {
        TryToCleanUpQuickshotAnimationToPreventArmOutstretchedBug();
    }
    g_PlayerQuickshot.reset();
}
class SharedPtrAndSmth
{
public:
    SharedPtrNew<Entity>* shared; //0x0000
    char pad_0008[16]; //0x0008
}; //Size: 0x0018
assert_sizeof(SharedPtrAndSmth, 0x18);
class Functor_Parkour_Assassination_Entry : public FunctorBase
{
public:
    char pad_0100[232]; //0x0100
    SharedPtrAndSmth targetNPC; //0x01E8
    SharedPtrAndSmth secondOptionalTargetNPC_mb; //0x0200
    char pad_0218[152]; //0x0218
}; //Size: 0x02B0
assert_sizeof(Functor_Parkour_Assassination_Entry, 0x2B0);
void PlayerJustStartedAssassinationAttempt(Functor_Parkour_Assassination_Entry& parkourAssassinationState)
{
    g_PlayerAssassinationAttemptState.emplace();
    g_PlayerAssassinationAttemptState->targetNPC = parkourAssassinationState.targetNPC.shared->GetPtr();
    g_PlayerAssassinationAttemptState->secondOptionalTargetNPC = parkourAssassinationState.secondOptionalTargetNPC_mb.shared->GetPtr();
    if (g_PlayerQuickshot)
    {
        g_PlayerQuickshot->m_isAssassinationStartedAfterQuickshotStarted = true;
    }
}
void PlayerJustEndedAssassinationAttempt()
{
    g_PlayerAssassinationAttemptState.reset();
    if (g_PlayerQuickshot)
    {
        g_PlayerQuickshot->m_isAssassinationEndedAfterQuickshotStarted = true;
    }
}
bool BetterQuickshot_IsPlayerInAssassination()
{
    return g_PlayerAssassinationAttemptState.has_value();
}
#include "ACU/CLAssassin.h"
#include "ACU/Clock.h"
void PlayerHasJustMadeRangedWeaponShot()
{
    if (!g_PlayerQuickshot)
    {
        return;
    }
    g_PlayerQuickshot->m_isShotDischargedYet = true;
    if (!g_PlayerAssassinationAttemptState) { return; }
    CLAssassin* cla = CLAssassin::GetSingleton();
    if (!cla) { return; }
    HasLanterndlcComponent* hasQuickshotEndTimer = cla->hasLanternCpnt;
    if (!hasQuickshotEndTimer) { return; }
    Timer& quickshotEndTimer = hasQuickshotEndTimer->timer_ReholsterAfterQuickshot;
    constexpr uint64 GAME_TIMER_INTEGER_TICKS_PER_SECOND = 30000;
    quickshotEndTimer.timestampEnd = quickshotEndTimer.clock->currentTimestamp;
    if (!g_PlayerQuickshot->m_isAssassinationStartedAfterQuickshotStarted)
    {
        // If the Quickshot was started _before_ Assassination, then stop the Quickshot animation
        // as soon as the weapon is discharged.
        // If the Quickshot was started _while_ Assassination is going on,
        // then let the animation play out for a brief moment after discharge.
        quickshotEndTimer.timestampEnd += GAME_TIMER_INTEGER_TICKS_PER_SECOND * 0.3f;
    }
}
bool BetterQuickshot_IsWaitingForQuickshotToComplete()
{
    if (!g_PlayerQuickshot)
    {
        return false;
    }
    if (BetterQuickshot_IsPlayerInAssassination())
    {
        return true;
        return !g_PlayerQuickshot->m_isShotDischargedYet;
    }
    return true;
}

void Functor_Quickshot_Enter__hook(AllRegisters* params)
{
    Functor_Quickshot* functorQuickshot = (Functor_Quickshot*)params->rcx_;
    HumanStatesHolder* humanStates = functorQuickshot->GetNthParent<HumanStatesHolder>(2);
    Entity* ownerCharacter = humanStates->ownerEntity;
    if (ownerCharacter && ownerCharacter == ACU::GetPlayer())
    {
        PlayerHasJustEnteredQuickshot(*functorQuickshot);
    }
}
void Functor_Quickshot_Exit__hook(AllRegisters* params)
{
    Functor_Quickshot* functorQuickshot = (Functor_Quickshot*)params->rcx_;
    HumanStatesHolder* humanStates = functorQuickshot->GetNthParent<HumanStatesHolder>(2);
    Entity* ownerCharacter = humanStates->ownerEntity;
    if (ownerCharacter && ownerCharacter == ACU::GetPlayer())
    {
        PlayerHasJustExitedQuickshot();
    }
}
void WhenAboutToCreateAGunshot_RememberTime(AllRegisters* params)
{
    auto humanStates = (HumanStatesHolder*)params->rcx_;
    Entity* ownerCharacter = humanStates->ownerEntity;
    if (ownerCharacter && ownerCharacter == ACU::GetPlayer())
    {
        PlayerHasJustMadeRangedWeaponShot();
    }
}
void WhenAssassinationAttemptStarted(AllRegisters* params)
{
    auto* parkourAssassinationState = (Functor_Parkour_Assassination_Entry*)params->rcx_;
    auto humanStates = parkourAssassinationState->GetNthParent<HumanStatesHolder>(2);
    Entity* ownerCharacter = humanStates->ownerEntity;
    if (ownerCharacter && ownerCharacter == ACU::GetPlayer())
    {
        PlayerJustStartedAssassinationAttempt(*parkourAssassinationState);
    }
}
void WhenAssassinationAttemptEnded(AllRegisters* params)
{
    auto humanStates = ((FunctorBase*)params->rcx_)->GetNthParent<HumanStatesHolder>(2);
    Entity* ownerCharacter = humanStates->ownerEntity;
    if (ownerCharacter && ownerCharacter == ACU::GetPlayer())
    {
        PlayerJustEndedAssassinationAttempt();
    }
}
void WhenTimerToEndQuickshotEnds_Remember(AllRegisters* params)
{
    if (g_PlayerQuickshot)
    {
        g_PlayerQuickshot->m_isSupposedToHaveEndedAlready = true;
    }
}
#include "ACU/CLAssassin.h"
void WhenCheckingIfTimerToEndQuickshotIsActive_ReactivateIfRecentlyFailed(AllRegisters* params)
{
    auto* hasQSEndTimer = (HasLanterndlcComponent*)params->rsi_;
    if (hasQSEndTimer->timer_ReholsterAfterQuickshot.isActive_mb_20)
    {
        // Don't change anything.
        return;
    }
    // Timer is deactivated. Check if reholstering failed, and if it did, reactivate the timer.
    if (!g_PlayerQuickshot)
    {
        return;
    }
    if (!g_PlayerQuickshot->m_isSupposedToHaveEndedAlready)
    {
        return;
    }
    auto* humanStates = (HumanStatesHolder*)params->rbp_;
    auto* callbacksForEndingQuickshot = humanStates->callbacksForIdx[639];
    if (!callbacksForEndingQuickshot)
    {
        return;
    }
    if (!callbacksForEndingQuickshot->callbacksWithReceivers.size)
    {
        return;
    }
    Functor_Quickshot* unendedQS = (Functor_Quickshot*)callbacksForEndingQuickshot->callbacksWithReceivers[0].humanStateNode;
    if (!unendedQS)
    {
        return;
    }
    if (&g_PlayerQuickshot->m_functorQS == unendedQS)
    {
        hasQSEndTimer->timer_ReholsterAfterQuickshot.isActive_mb_20 = true;
    }
}
void WhenGettingRangedWeaponTarget_onWallInJumpEtc_ForceScan(AllRegisters* params);
DEFINE_GAME_FUNCTION(sub_141B034A0, 0x141B034A0, char, __fastcall, (FunctorBase* a1, FunctorBase* a2));
DEFINE_GAME_FUNCTION(sub_1419FA390, 0x1419FA390, char, __fastcall, (HumanStatesHolder* p_humanStates, unsigned int edx0, __int64 a3));
bool g_MoreReliableQuickshot_DuringAssassination_IsTryingToStartAQuickshotNow = false;
void WhenTryingToMakeAQuickshotAndLeadingToAnAllowUnsheathingCheck_RememberTheContextOfQuickshotAttempt(AllRegisters* params)
{
    g_MoreReliableQuickshot_DuringAssassination_IsTryingToStartAQuickshotNow = true;
    // There is a hook several calls down that enables the player
    // to initiate Quickshots during assassination.
    // A side effect of that is being allowed to unsheathe the melee weapon
    // when sitting on ledges, but in a bad way
    // (some of my other patches cause it to be immediately resheathed).
    // Here I just provide the information to the "deeper" hook that
    // "I'm trying to start a quickshot, not unsheathe a melee weapon".
    sub_1419FA390((HumanStatesHolder*)params->rcx_, (unsigned int&)params->rdx_, params->r8_);
    g_MoreReliableQuickshot_DuringAssassination_IsTryingToStartAQuickshotNow = false;
}
void WhenPerformingSomeOtherCheckAboutWhetherQuickshotIsAllowedToStart_AllowInitiateQuickshotDuringAssassination(AllRegisters* params)
{
    char isUnsheatheNormallyAllowedToStartHere = sub_141B034A0((FunctorBase*)params->rcx_, (FunctorBase*)params->rdx_);
    params->GetRAX() = isUnsheatheNormallyAllowedToStartHere;
    if (isUnsheatheNormallyAllowedToStartHere)
    {
        return;
    }
    if (!g_MoreReliableQuickshot_DuringAssassination_IsTryingToStartAQuickshotNow) { return; }
    if (!BetterQuickshot_IsLessRestrictionsEnabled())
    {
        return;
    }
    // If trying to unholster a ranged weapon for a quickshot, allow to proceed.
    params->GetRAX() = 1;
}
void WhenGunshotRaycastSuccessful_Display(AllRegisters* params);
void WhenGatheredGunshotLineOfFireResults_RememberResults(AllRegisters* params);
void WhenTheFirstGunshotRaycastResultWasSelected_DontCollideWithTheChokedNPC(AllRegisters* params);
MoreReliableQuickshot::MoreReliableQuickshot()
{
    //auto PreventAutomaticInstantReholsteringInMostSituations_v1 = [&]()
    //{
    //    DEFINE_ADDR(whenQuickshotIsInterruptedByParkourUnparentGunFromHand, 0x141AB3480);
    //    whenQuickshotIsInterruptedByParkourUnparentGunFromHand = {
    //        0xC3        // ret
    //    };
    //};
    auto PreventAutomaticInstantReholsteringInMostSituations_v3 = [&]()
    {
        // Triggers every frame when you're in state that would normally prevent you from having a gun out:
        // during assassinations, jumps, while on wall, etc.
        const uintptr_t whenInstantSheathingOrReholsteringDueToParkour = 0x141AB34F9;
        PresetScript_CCodeInTheMiddle(whenInstantSheathingOrReholsteringDueToParkour, 5,
            WhenInstantSheathingOrReholsteringDueToParkour_DontDoThat, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    };
    auto PreventAutomaticInstantReholstering_AssassinationStart = [&]()
    {
        // Triggers once when just starting an "assassination attempt":
        // choke, failed assassination from above do also count.
        const uintptr_t whenInstantSheathingOrReholsteringDueToAssassination = 0x141AB3E74;
        PresetScript_CCodeInTheMiddle(whenInstantSheathingOrReholsteringDueToAssassination, 5,
            WhenInstantSheathingOrReholsteringDueToAssassinationStart_DontDoThat, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    };
    auto FixImaginaryReloadIfQuickshotWhileSheathing = [&]()
    {
        const uintptr_t whenFinalConfirmationThatQuickshotIsToBeAttempted = 0x14265E077;
        PresetScript_NOP(whenFinalConfirmationThatQuickshotIsToBeAttempted, 2);
    };

    auto HookQuickshotStateLifetime = [&]()
    {
        PresetScript_CCodeInTheMiddle(0x141AA4DD0, 9,
            Functor_Quickshot_Enter__hook, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
        PresetScript_CCodeInTheMiddle(0x141AA5430, 8,
            Functor_Quickshot_Exit__hook, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };


    auto HookQuickshotMomentOfDischarge = [&]()
    {
        const uintptr_t whenAboutToCreateAGunshot = 0x141A0D918;
        PresetScript_CCodeInTheMiddle(whenAboutToCreateAGunshot, 6,
            WhenAboutToCreateAGunshot_RememberTime, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };
    auto HookAssassinationStateLifetime = [&]()
    {
        const uintptr_t Functor_Parkour_Assassination_Entry__Enter = 0x141A462F0;
        const uintptr_t Functor_Parkour_Assassination_Entry__Exit = 0x141A464A0;
        PresetScript_CCodeInTheMiddle(Functor_Parkour_Assassination_Entry__Enter, 5,
            WhenAssassinationAttemptStarted, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
        PresetScript_CCodeInTheMiddle(Functor_Parkour_Assassination_Entry__Exit, 5,
            WhenAssassinationAttemptEnded, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };

    auto FixForGettingStuckUnableToMoveOnLedgesOrChangeRangedWeapon_HookTimerThatEndsQuickshot = [&]()
    {
        const uintptr_t whenTimerToEndQuickshotEnds = 0x142662327;
        PresetScript_CCodeInTheMiddle(whenTimerToEndQuickshotEnds, 7,
            WhenTimerToEndQuickshotEnds_Remember, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
        const uintptr_t whenCheckingIfTimerToEndQuickshotIsActive = 0x1426622D5;
        PresetScript_CCodeInTheMiddle(whenCheckingIfTimerToEndQuickshotIsActive, 7,
            WhenCheckingIfTimerToEndQuickshotIsActive_ReactivateIfRecentlyFailed, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };

    auto AllowScanForQuickshotTargetFromPeaksAndVShapes = [&]()
    {
        DEFINE_ADDR(whenUpdatingRangedWeaponTarget_sitOnPeak, 0x141A03660);
        DEFINE_ADDR(whenUpdatingRangedWeaponTarget_sitOnLedge, 0x141A14B30);
        whenUpdatingRangedWeaponTarget_sitOnPeak = {
            "48 8B 49 28"                                           // - mov rcx,[rcx+28]
            "48 8B 49 08"                                           // - mov rcx,[rcx+08]
            "E9", RIP(whenUpdatingRangedWeaponTarget_sitOnLedge)    // - jmp ACU.exe+1A14B30
        };
    };
    auto AllowScanForQuickshotTargetInMostSituations = [&]()
    {
        uintptr_t whenUpdatingRangedWeaponTarget_onWallInJumpEtc_fnepilogue = 0x141AAD9EF;
        PresetScript_CCodeInTheMiddle(whenUpdatingRangedWeaponTarget_onWallInJumpEtc_fnepilogue, 5,
            WhenGettingRangedWeaponTarget_onWallInJumpEtc_ForceScan, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };
    auto AnotherPreventionOfTheArmOutstretchedBug = [&]()
    {
        uintptr_t someConditionThatLeadsToReholsterCheck = 0x1426687B7;
        PresetScript_NOP(someConditionThatLeadsToReholsterCheck, 2);
    };
    //auto AlsoAllowInitiateQuickshotDuringAssassination = [&]()
    //{
    //    DEFINE_ADDR(whenDoingSomeOtherCheckAboutWhetherQuickshotAnimationIsAllowed, 0x141B054B4);

    //    // I've been hunting these two bytes for about a month.
    //    whenDoingSomeOtherCheckAboutWhetherQuickshotAnimationIsAllowed = {
    //        0xEB, 0x44          // - jmp ACU.exe+1B054FA
    //    };
    //};
    auto AlsoAllowInitiateQuickshotDuringAssassination_v2 = [&]()
    {
        uintptr_t whenPerformingSomeOtherCheckAboutWhetherQuickshotIsAllowedToStart = 0x141B054AD;
        uintptr_t whenTryingToMakeAQuickshotAndLeadingToAnAllowUnsheathingCheck = 0x14265E03B;
        PresetScript_CCodeInTheMiddle(whenPerformingSomeOtherCheckAboutWhetherQuickshotIsAllowedToStart, 5,
            WhenPerformingSomeOtherCheckAboutWhetherQuickshotIsAllowedToStart_AllowInitiateQuickshotDuringAssassination, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
        PresetScript_CCodeInTheMiddle(whenTryingToMakeAQuickshotAndLeadingToAnAllowUnsheathingCheck, 5,
            WhenTryingToMakeAQuickshotAndLeadingToAnAllowUnsheathingCheck_RememberTheContextOfQuickshotAttempt, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    };



    HookQuickshotStateLifetime();
    FixForGettingStuckUnableToMoveOnLedgesOrChangeRangedWeapon_HookTimerThatEndsQuickshot();

    FixImaginaryReloadIfQuickshotWhileSheathing();
    AllowScanForQuickshotTargetFromPeaksAndVShapes();

    HookQuickshotMomentOfDischarge();
    HookAssassinationStateLifetime();

    //PreventAutomaticInstantReholsteringInMostSituations_v1();
    PreventAutomaticInstantReholsteringInMostSituations_v3();
    PreventAutomaticInstantReholstering_AssassinationStart();

    AllowScanForQuickshotTargetInMostSituations();

    //AlsoAllowInitiateQuickshotDuringAssassination();
    AlsoAllowInitiateQuickshotDuringAssassination_v2();

    AnotherPreventionOfTheArmOutstretchedBug();



    auto AWayToMonitorTheCollisionResultsOfTheGunshots = [&]()
    {
        uintptr_t whenGunshotRaycastSuccessful = 0x14057659E;
        PresetScript_CCodeInTheMiddle(whenGunshotRaycastSuccessful, 5,
            WhenGunshotRaycastSuccessful_Display, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
        uintptr_t whenGatheredGunshotLineOfFireResults = 0x140579252;
        PresetScript_CCodeInTheMiddle(whenGatheredGunshotLineOfFireResults, 5,
            WhenGatheredGunshotLineOfFireResults_RememberResults, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };
    auto DontAccidentallyHitTheChokedNPCWhenPerformingQuickshotDuringChoke = [&]()
    {
        uintptr_t whenTheFirstRaycastResultWasSelected = 0x140579268;
        PresetScript_CCodeInTheMiddle(whenTheFirstRaycastResultWasSelected, 6,
            WhenTheFirstGunshotRaycastResultWasSelected_DontCollideWithTheChokedNPC, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };

    //AWayToMonitorTheCollisionResultsOfTheGunshots();
    DontAccidentallyHitTheChokedNPCWhenPerformingQuickshotDuringChoke();
}




static bool IsShouldForceAllowScanForQuickshotTargets()
{
    if (IsPlayerParkouringBothHandsBusy())
    {
        if (IsCurrentRangedWeaponTwohanded())
        {
            return false;
        }
    }
    return true;
}
DEFINE_GAME_FUNCTION(sitOnLedge_writeRangedTargetEntity, 0x141A14B30, void, __fastcall, (__int64 a1, __int64 a2))
void WhenGettingRangedWeaponTarget_onWallInJumpEtc_ForceScan_inner(__int64 a1, __int64 a2)
{
    struct Fake_Functor_Parkour_Nav_Entry__ParentStack
    {
        void* p0 = nullptr;
        void* p8 = nullptr;
        HumanStatesHolder* humanStatesRoot;
    };
    assert_offsetof(Fake_Functor_Parkour_Nav_Entry__ParentStack, humanStatesRoot, 0x10);
    __declspec(align(0x10)) struct Fake_Functor_Parkour_Nav_Entry
    {
        char pad_0[0x28] = { 0 };
        Fake_Functor_Parkour_Nav_Entry__ParentStack* parentStack;
        char pad_30[0x200 - 0x30] = { 0 };
        Vector4f playerForward;
        Fake_Functor_Parkour_Nav_Entry__ParentStack fakeParentStackInstance;
    };
    assert_offsetof(Fake_Functor_Parkour_Nav_Entry, playerForward, 0x200);

    Fake_Functor_Parkour_Nav_Entry fakeFunctor;

    HumanStatesHolder* humanStates = (HumanStatesHolder*)a1;
    fakeFunctor.fakeParentStackInstance.humanStatesRoot = humanStates;
    fakeFunctor.playerForward = (Vector4f&)humanStates->ownerEntity->mainTransform[4 * 1];
    fakeFunctor.parentStack = &fakeFunctor.fakeParentStackInstance;

    sitOnLedge_writeRangedTargetEntity((__int64)&fakeFunctor, a2);
}
void WhenGettingRangedWeaponTarget_onWallInJumpEtc_ForceScan(AllRegisters* params)
{
    if (!BetterQuickshot_IsLessRestrictionsEnabled())
    {
        return;
    }
    if (!IsShouldForceAllowScanForQuickshotTargets())
    {
        return;
    }
    WhenGettingRangedWeaponTarget_onWallInJumpEtc_ForceScan_inner(params->rdi_, params->rsi_);
}



#include "ImGui3D.h"
void WhenGunshotRaycastSuccessful_Display(AllRegisters* params)
{
    Entity* collidedEntity = *(Entity**)(params->rbp_ - 0x80);
    if (!collidedEntity) { return; }
    ImGui3D::DrawLocationNamed(collidedEntity->GetPosition(), "Last Gunshot target Entity");
    Vector3f* hitLocation = (Vector3f*)(params->rbp_ + 0xB0);
    ImGui3D::DrawLocationNamed(*hitLocation, "Last Gunshot hit location");
}
class PhysicComponent;
class RaycastResult_mb
{
public:
    Vector4f hitLocation; //0x0000
    Vector4f targetNormal; //0x0010
    PhysicComponent* physicCpnt; //0x0020
    SharedPtrNew<Entity>* shared_targetEntity; //0x0028
    uint32 entityId_mb; //0x0030
    char pad_0034[4]; //0x0034
    float distance_mb; //0x0038
    char pad_003C[12]; //0x003C
    uint32 dword_48; //0x0048
    char pad_004C[20]; //0x004C
}; //Size: 0x0060
assert_sizeof(RaycastResult_mb, 0x60);
struct MyRememberedRaycastResult
{
    Entity* targetEntity;
    Vector4f hitLocation;
    Vector4f targetNormal;
    float distance;
};
std::vector<MyRememberedRaycastResult> g_LastGunshotRaycastResults;
Entity* GetChokedEntity();
Entity* GetQuickshotTargetEntity();
void WhenGatheredGunshotLineOfFireResults_RememberResults(AllRegisters* params)
{
    SmallArray<RaycastResult_mb>& arrResults = *(SmallArray<RaycastResult_mb>*)(params->GetRSP() + 0x68);
    g_LastGunshotRaycastResults.clear();
    for (RaycastResult_mb& res : arrResults)
    {
        g_LastGunshotRaycastResults.push_back({
            res.shared_targetEntity->GetPtr(),
            res.hitLocation,
            res.targetNormal,
            res.distance_mb
            });
    }
}
DEFINE_GAME_FUNCTION(ArrRaycastResults__GetNext, 0x1425D5550, RaycastResult_mb*, __fastcall, (SmallArray<RaycastResult_mb>* a1, RaycastResult_mb* p_startFrom_opt, char p_0getFirst));
void WhenTheFirstGunshotRaycastResultWasSelected_DontCollideWithTheChokedNPC(AllRegisters* params)
{
    RaycastResult_mb* firstSelectedRaycastResult = (RaycastResult_mb*)params->GetRAX();
    if (!firstSelectedRaycastResult)
    {
        return;
    }
    Entity* raycastResultTargetEntity = firstSelectedRaycastResult->shared_targetEntity->GetPtr();
    Entity* chokedEntity = GetChokedEntity();
    Entity* quickshotTarget = GetQuickshotTargetEntity();
    if (!chokedEntity)
    {
        // No collisions need to be skipped.
        return;
    }
    if (quickshotTarget == chokedEntity)
    {
        // The player kind of intended to hit the NPC he chokes.
        // Probably because they started a quickshot and then immediately started an assassination on the same NPC.
        // Probably didn't really intend to do that,
        // but it's better to let the NPC absorb the bullet than to accidentally shoot through the NPC
        // and into the crowd (if you _do_ make the bullet pass through, then you sometimes
        // get to accidentally shoot a _different_ enemy, which feels cool but unpredictable).
        return;
    }
    if (raycastResultTargetEntity != chokedEntity)
    {
        // Didn't collide with the choked NPC, just the way it should be.
        return;
    }
    // The choked NPC will (accidentally) absorb the bullet unless I change that now.
    SmallArray<RaycastResult_mb>& arrResults = *(SmallArray<RaycastResult_mb>*)(params->GetRSP() + 0x68);
    RaycastResult_mb* it = firstSelectedRaycastResult;
    RaycastResult_mb* selectedResult = nullptr;
    do
    {
        it = ArrRaycastResults__GetNext(&arrResults, it, 0);
        if (!it)
        {
            // Found no satisfactory raycast results. So scratch all that, don't change the selected result.
            return;
        }
        if (it->shared_targetEntity->GetPtr() != chokedEntity)
        {
            // Collide with the next entity after the choked NPC.
            selectedResult = it;
            break;
        }
    } while (true);
    params->GetRAX() = (uint64)selectedResult;
}

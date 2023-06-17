#include "pch.h"

#include "Hack_MoreReliableQuickshot.h"
#include "ACU/Entity.h"
#include "ACU/HumanStatesHolder.h"
#include "ACU/CSrvPlayerWeaponSwitch.h"
#include "ACU/ACUGetSingletons.h"

#include "ACU_DefineNativeFunction.h"

bool g_BetterQuickshot_LessRestrictions = true;

bool IsPlayerParkouringBothHandsBusy();
bool IsCurrentRangedWeaponTwohanded();
static bool BetterQuickshot_IsPlayerInAssassination();
static bool BetterQuickshot_IsWaitingForQuickshotToComplete();
// Triggers every frame when you're in state that would normally prevent you from having a gun out:
// during assassinations, jumps, while on wall, etc.
static bool BetterQuickshot_IsShouldAllowToProceedWithoutReholstering_Parkour(int p_1melee2ranged)
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
// Triggers once when just starting an "assassination attempt":
// choke, failed assassination from above do also count.
static bool BetterQuickshot_IsShouldAllowToProceedWithoutReholstering_AssassinationStart()
{
    return true;
}
DEFINE_GAME_FUNCTION(ReattachWeaponToSheathOrHolster, 0x141B05570, void, __fastcall, (__int64 a1, int p_1melee2ranged));
bool WhenInstantSheathingOrReholsteringDueToParkour_IsShouldSkipReholstering(Entity* ownerEntity, int p_1melee2ranged)
{
    const bool isPlayer = ownerEntity && ownerEntity == ACU::GetPlayer();
    if (!isPlayer)
    {
        return false;
    }
    return BetterQuickshot_IsShouldAllowToProceedWithoutReholstering_Parkour(p_1melee2ranged);
}
void WhenInstantSheathingOrReholsteringDueToParkour_DontDoThat(AllRegisters* params)
{
    if (!g_BetterQuickshot_LessRestrictions)
    {
        ReattachWeaponToSheathOrHolster(params->rcx_, params->rdx_);
        return;
    }
    const int p_1melee2ranged = (int&)params->rdx_;
    FunctorBase* node = (FunctorBase*)params->rcx_;
    auto* humanStates = node->GetNthParent<HumanStatesHolder>(0);
    Entity* ownerEntity = humanStates->ownerEntity;
    if (WhenInstantSheathingOrReholsteringDueToParkour_IsShouldSkipReholstering(ownerEntity, p_1melee2ranged))
    {
        return;
    }
    ReattachWeaponToSheathOrHolster(params->rcx_, params->rdx_);
}
void WhenInstantSheathingOrReholsteringDueToAssassinationStart_DontDoThat(AllRegisters* params)
{
    if (!g_BetterQuickshot_LessRestrictions)
    {
        ReattachWeaponToSheathOrHolster(params->rcx_, params->rdx_);
        return;
    }
    const bool allowToProceedWithoutReholstering = BetterQuickshot_IsShouldAllowToProceedWithoutReholstering_AssassinationStart();
    if (!allowToProceedWithoutReholstering)
    {
        ReattachWeaponToSheathOrHolster(params->rcx_, params->rdx_);
        return;
    }
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
struct PlayerQuickshotState
{
    Functor_Quickshot& m_functorQS;
    bool m_isShotDischargedYet = false;
    bool m_isSupposedToHaveEndedAlready = false;
    PlayerQuickshotState(Functor_Quickshot& functorQS) : m_functorQS(functorQS) {}
};
struct PlayerAssassinationAttemptState
{

};
std::optional<PlayerAssassinationAttemptState> g_PlayerAssassinationAttemptState;
std::optional<PlayerQuickshotState> g_PlayerQuickshot;
void PlayerHasJustEnteredQuickshot(Functor_Quickshot& functorQS)
{
    g_PlayerQuickshot.emplace(functorQS);
}
void PlayerHasJustExitedQuickshot()
{
    g_PlayerQuickshot.reset();
}
void PlayerJustStartedAssassinationAttempt()
{
    g_PlayerAssassinationAttemptState.emplace();
}
void PlayerJustEndedAssassinationAttempt()
{
    g_PlayerAssassinationAttemptState.reset();
}
bool BetterQuickshot_IsPlayerInAssassination()
{
    return g_PlayerAssassinationAttemptState.has_value();
}
void PlayerHasJustMadeRangedWeaponShot()
{
    if (!g_PlayerQuickshot)
    {
        return;
    }
    g_PlayerQuickshot->m_isShotDischargedYet = true;
}
bool BetterQuickshot_IsWaitingForQuickshotToComplete()
{
    if (!g_PlayerQuickshot)
    {
        return false;
    }
    if (BetterQuickshot_IsPlayerInAssassination())
    {
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
    auto humanStates = ((FunctorBase*)params->rcx_)->GetNthParent<HumanStatesHolder>(2);
    Entity* ownerCharacter = humanStates->ownerEntity;
    if (ownerCharacter && ownerCharacter == ACU::GetPlayer())
    {
        PlayerJustStartedAssassinationAttempt();
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
        const uintptr_t whenInstantSheathingOrReholsteringDueToParkour = 0x141AB34F9;
        PresetScript_CCodeInTheMiddle(whenInstantSheathingOrReholsteringDueToParkour, 5,
            WhenInstantSheathingOrReholsteringDueToParkour_DontDoThat, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    };
    auto PreventAutomaticInstantReholstering_AssassinationStart = [&]()
    {
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
    if (!g_BetterQuickshot_LessRestrictions)
    {
        return;
    }
    if (!IsShouldForceAllowScanForQuickshotTargets())
    {
        return;
    }
    WhenGettingRangedWeaponTarget_onWallInJumpEtc_ForceScan_inner(params->rdi_, params->rsi_);
}

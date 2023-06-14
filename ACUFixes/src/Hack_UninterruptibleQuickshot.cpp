#include "pch.h"

#include "Hack_UninterruptibleQuickshot.h"
#include "ACU/Entity.h"
#include "ACU/HumanStatesHolder.h"
#include "ACU/CSrvPlayerWeaponSwitch.h"

#include "ACU_DefineNativeFunction.h"






UninterruptibleQuickshot::UninterruptibleQuickshot()
{
    auto PreventAutomaticInstantReholsteringInMostSituations_v1 = [&]()
    {
        DEFINE_ADDR(whenQuickshotIsInterruptedByParkourUnparentGunFromHand, 0x141AB3480);
        whenQuickshotIsInterruptedByParkourUnparentGunFromHand = {
            0xC3        // ret
        };
    };
    auto PreventAutomaticInstantReholsteringInMostSituations_v2 = [&]()
    {
        PresetScript_NOP(0x141AB34F9, 5);
    };
    auto FixImaginaryReloadIfQuickshotWhileSheathing = [&]()
    {
        const uintptr_t whenFinalConfirmationThatQuickshotIsToBeAttempted = 0x14265E077;
        PresetScript_NOP(whenFinalConfirmationThatQuickshotIsToBeAttempted, 2);
    };

    //PreventAutomaticInstantReholsteringInMostSituations_v1();
    PreventAutomaticInstantReholsteringInMostSituations_v2();
    FixImaginaryReloadIfQuickshotWhileSheathing();
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
    fakeFunctor.playerForward = (Vector4f&)humanStates->player->mainTransform[4 * 1];
    fakeFunctor.parentStack = &fakeFunctor.fakeParentStackInstance;

    sitOnLedge_writeRangedTargetEntity((__int64)&fakeFunctor, a2);
}
bool IsPlayerClimbingOnWall()
{
    auto* humanStates = HumanStatesHolder::GetForPlayer();
    if (!humanStates) {
        return false;
    }
    auto* navigationCallbacks = humanStates->callbacksForIdx[366];
    if (!navigationCallbacks) {
        return false;
    }
    if (!navigationCallbacks->arrThoseFnsElem.size) { return false; }
    uint64 WhileOnWallSystem_VTable = 0x142FFC970;
    uint64 WhileInVerticalWallrun_VTable = 0x142FFCAA8;
    uint64 WhileInSwingHang_VTable = 0x142FFCB20;
    uint64 currentParkourModeVTable = *(uint64*)(navigationCallbacks->arrThoseFnsElem[0].nonstaticNode);
    return currentParkourModeVTable == WhileOnWallSystem_VTable
        || currentParkourModeVTable == WhileInVerticalWallrun_VTable
        || currentParkourModeVTable == WhileInSwingHang_VTable
        ;
}
CSrvPlayerWeaponSwitch* FindPlayerWeaponSwitch();
static bool IsShouldForceAllowQuickshot()
{
    if (IsPlayerClimbingOnWall())
    {
        CSrvPlayerWeaponSwitch* weaponSwitcher = FindPlayerWeaponSwitch();
        if (weaponSwitcher)
        {
            EquipmentType rangedWeaponType = weaponSwitcher->equipType_LeftHand;
            if (rangedWeaponType == EquipmentType::Rifle
                || rangedWeaponType == EquipmentType::GuillotineGun
                || rangedWeaponType == EquipmentType::Blunderbuss
                )
            {
                return false;
            }
        }
    }
    return true;
}
void WhenGettingRangedWeaponTarget_onWallInJumpEtc_ForceScan(AllRegisters* params)
{
    if (!IsShouldForceAllowQuickshot())
    {
        return;
    }
    WhenGettingRangedWeaponTarget_onWallInJumpEtc_ForceScan_inner(params->rdi_, params->rsi_);
}
QuickshotTargettingWhenSittingOnPeaks::QuickshotTargettingWhenSittingOnPeaks()
{
    DEFINE_ADDR(whenUpdatingRangedWeaponTarget_sitOnPeak, 0x141A03660);
    DEFINE_ADDR(whenUpdatingRangedWeaponTarget_sitOnLedge, 0x141A14B30);
    whenUpdatingRangedWeaponTarget_sitOnPeak = {
        "48 8B 49 28"                                           // - mov rcx,[rcx+28]
        "48 8B 49 08"                                           // - mov rcx,[rcx+08]
        "E9", RIP(whenUpdatingRangedWeaponTarget_sitOnLedge)    // - jmp ACU.exe+1A14B30
    };

    uintptr_t whenUpdatingRangedWeaponTarget_onWallInJumpEtc_fnepilogue = 0x141AAD9EF;
    PresetScript_CCodeInTheMiddle(whenUpdatingRangedWeaponTarget_onWallInJumpEtc_fnepilogue, 5,
        WhenGettingRangedWeaponTarget_onWallInJumpEtc_ForceScan, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

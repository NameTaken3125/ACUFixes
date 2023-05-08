#include "pch.h"

#include "Hack_MoreResponsiveBombQuickDrop.h"


#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"
#include "ACU_DefineNativeFunction.h"
#include "ACU_InputUtils.h"





bool g_BombQuickthrowEnabler_bombDropCheckerHasBeenForceEnabledThisFrame = false;
bool g_BombQuickthrowEnabler_isDroppingBombAnimationNow = false;
bool g_BombQuickthrowEnabler_isStartedBombDropThisFrame = false;
void SetIsDropping(bool isDroppingNow)
{
    g_BombQuickthrowEnabler_isDroppingBombAnimationNow = isDroppingNow;
}
bool g_BombQuickthrowEnabler_isNeedToFixRightArm = false;
static bool BombQuickthrowEnabler_isNeedToFixRightArm()
{
    return g_BombQuickthrowEnabler_isNeedToFixRightArm;
}
bool g_BombQuickthrowEnabler_isLevelJustReloaded = false;
void WhenCheckingIfBombShouldBeQuickDropped_ForceIfSprinting_inner(AllRegisters* params)
{
    bool doBombDrop = params->r15_ & 0xFF;
    if (doBombDrop)
        return;
    const bool forceBombDrop
        = ACU::Input::IsPressed(ActionKeyCode::BombDrop)
        && ACU::Input::IsPressed(ActionKeyCode::Sprint);
        //= false;
    params->r15_ = forceBombDrop;
}
void WhenCheckingIfBombShouldBeQuickDropped_ForceIfSprinting(AllRegisters* params)
{
    WhenCheckingIfBombShouldBeQuickDropped_ForceIfSprinting_inner(params);
    const bool doBombDrop = params->r15_ & 0xFF;
    if (!doBombDrop)
        return;
}
LessFinickyBombQuickDrop::LessFinickyBombQuickDrop()
{
    uintptr_t whenCheckingIfBombShouldBeQuickDropped = 0x142666F8F;
    PresetScript_CCodeInTheMiddle(whenCheckingIfBombShouldBeQuickDropped, 7,
        WhenCheckingIfBombShouldBeQuickDropped_ForceIfSprinting, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}








DEFINE_GAME_FUNCTION(SmallArray_DWORD__AppendIfNotPresent_mb, 0x140B8E8F0, void, __fastcall, (SmallArray<uint32>* arr, uint32* newElement));
void MoveElementToStart(SmallArray<uint32>& arr, uint32 elem)
{
    int initialPos = (int)(std::find(arr.begin(), arr.end(), elem) - arr.begin());
    for (int i = initialPos; i > 0; i--)
    {
        arr[i] = arr[i - 1];
    }
    arr[0] = elem;
}
void AddBombDropStateToListOfToBeUpdated(SmallArray<uint32>& arrayToFill)
{
    uint32 stateEnumThatChecksForBombDrop = 1;
    if (std::find(arrayToFill.begin(), arrayToFill.end(), stateEnumThatChecksForBombDrop) != arrayToFill.end())
    {
        return;
    }
    constexpr uint32 stateEnumThatIsUsedWhenLootingBodies = 14;
    if (std::find(arrayToFill.begin(), arrayToFill.end(), stateEnumThatIsUsedWhenLootingBodies) != arrayToFill.end())
    {
        return;
    }
    SmallArray_DWORD__AppendIfNotPresent_mb(&arrayToFill, &stateEnumThatChecksForBombDrop);
    {
        // UPD: I'm now having doubts regarding what's written below about preserving the original array's order.
        //      State `1` _does_ have to be placed at the start though.
        //
        // This here is extremely important.
        // Without this, when in combat, Arno dropping a bomb during a roll or an attack
        // uses not just the left arm to drop the bomb, but also the right arm and really the whole upper body
        // stays mostly upright. This is especially weird (spine breaking weird) during a roll.
        // I used to just move `stateEnumThatChecksForBombDrop` to the start by swapping with the first element,
        // but that wasn't enough.
        // In combat, the state enums to be updated are normally [0, 2].
        // To allow bomb drops (and fix animations), the resulting array must be
        // [1, 0, 2]. Again, order matters, because for example if array is left as [1, 2, 0], then only [1, 2]
        // will be updated (states updater will short-return after `2` because it returns `true`),
        // and to unbreak the spine I _need_ to run state `0` with `isInHighProfile==true` _in_the_same_frame_.
        MoveElementToStart(arrayToFill, stateEnumThatChecksForBombDrop);
    }
    g_BombQuickthrowEnabler_bombDropCheckerHasBeenForceEnabledThisFrame = true;
}
void WhenFillingArrayOfStateEnumsToUpdate_EnableChecksForBombQuickDrop(AllRegisters* params)
{
    auto& arrayToFill = *(SmallArray<uint32>*)(params->GetRSP() + 0x30);
    if (arrayToFill.size == 2
        && arrayToFill[0] == 0
        && arrayToFill[1] == 2
        )
    {
        // The combat situation. There are some confusing and hard to catch bugs if enabled in combat.
        // _Sometimes_, under undetermined conditions, perhaps after rolling around a lot,
        // dropping bombs and quickshooting a lot, Arno gets stuck in a state where he can't change ranged weapon,
        // and shooting itself feels, um, weird.
        // Exiting and reentering combat (weapon needs to be completely sheathed, I guess)
        // restores normalcy, but this is not really acceptable.
        // Perhaps if I'm able to also implement an always-available quickshot,
        // then "combat version" of quickshot can be removed just as the "combat version"
        // of quickthrow was.
        return;
    }
    if (arrayToFill.size == 3
        && arrayToFill[0] == 0
        && arrayToFill[1] == 6
        && arrayToFill[2] == 3
        )
    {
        // Sitting behind cover. Not needed, perhaps (and enabling it has a side effect of
        // `Loot` interaction pop up near dead bodies).
        return;
    }
    AddBombDropStateToListOfToBeUpdated(arrayToFill);
}


void WhenLevelReloaded_MakePreparationsToAvoidTPose(AllRegisters* params)
{
    g_BombQuickthrowEnabler_isLevelJustReloaded = true;
}
class HasLanterndlcComponent;
DEFINE_GAME_FUNCTION(DisableThisToMagicallyUnbreakAssassinationsAndDropBombs_C, 0x14265DAA0, int32, __fastcall, (HasLanterndlcComponent* a1, char a2));
void WhenUpdatingPlayerStatesMakingSomeStrangeCall_DoMagicToAllowQuickthrowAndAvoidTPose(AllRegisters* params)
{
    auto* someImportantObject = (HasLanterndlcComponent*)params->rcx_;
    char a2 = params->rdx_ & 0xFF;
    int32 strangeFunctionResult = DisableThisToMagicallyUnbreakAssassinationsAndDropBombs_C(someImportantObject, a2);
    if (strangeFunctionResult == 1)
    {
        const bool isLevelJustReloaded = g_BombQuickthrowEnabler_isLevelJustReloaded;
        if (isLevelJustReloaded)
        {
            // Leave it be. The level has just reloaded. If I change something here,
            // Arno will be left in T-pose.
            g_BombQuickthrowEnabler_isLevelJustReloaded = false;
        }
        else
        {
            strangeFunctionResult = 0;
        }
    }
    params->GetRAX() = strangeFunctionResult;
}
void HasLanternCpnt30SetHighProfile(uint64 hasLantern, bool makeInHighProfile)
{
    *(byte*)(*(uint64*)(hasLantern + 0x30) + 0x20) = makeInHighProfile;
}
void WhenDropBombCheckerFinishes_PretendArnoIsInHighProfileIfItsNeededToFixAnimations(AllRegisters* params)
{
    bool resultOfBombDropChecker_parentCall = (params->GetRAX()) & 0xFF;
    if (!resultOfBombDropChecker_parentCall)
        return;
    if (g_BombQuickthrowEnabler_isStartedBombDropThisFrame && BombQuickthrowEnabler_isNeedToFixRightArm())
    {
        auto* someImportantObject = (HasLanterndlcComponent*)params->rbx_;

        HasLanternCpnt30SetHighProfile(params->rbx_, true);
        params->rsi_ = 1;
        params->GetRAX() = 0;
    }
}
void WhenHighProfileMovementIsDecided_insideGetter_PretendArnoIsInHighProfileIfItsNeededToFixAnimations(AllRegisters* params)
{
    if (BombQuickthrowEnabler_isNeedToFixRightArm())
    {
        *(byte*)(params->rcx_ + 0x20) = 1;
    }
}
void WhenDropBombAnimationEnds_ClearBombDropFlags(AllRegisters* params)
{
    SetIsDropping(false);
    g_BombQuickthrowEnabler_isNeedToFixRightArm = false;
}
void WhenStatesUpdaterFinishes_ClearFlagsForFrame(AllRegisters* params)
{
    g_BombQuickthrowEnabler_bombDropCheckerHasBeenForceEnabledThisFrame = false;
    g_BombQuickthrowEnabler_isStartedBombDropThisFrame = false;
}
void WhenCheckingIfShouldDisallowDropBombOnSlopes_DisobeyOrders(AllRegisters* params)
{
    const bool isOnSlope_mb = *(bool*)(params->GetRSP() + 0x20);
    if (isOnSlope_mb)
        g_BombQuickthrowEnabler_isNeedToFixRightArm = true;
}
void WhenCheckingIfShouldDisallowDropBombInWallrunAndTrees_DisobeyOrders(AllRegisters* params)
{
    const bool isBombDropAllowed_falseifWallrunOrStandingInTree = *(bool*)(params->GetRSP() + 0x38);
    const bool needToFixRightArm = !isBombDropAllowed_falseifWallrunOrStandingInTree;
    if (needToFixRightArm)
        g_BombQuickthrowEnabler_isNeedToFixRightArm = true;
    params->rcx_ = params->rbx_;
}
void WhenSuccessfullyStartingToDropBomb_RememberSuccess(AllRegisters* params)
{
    SetIsDropping(true);
    g_BombQuickthrowEnabler_isStartedBombDropThisFrame = true;
    if (g_BombQuickthrowEnabler_bombDropCheckerHasBeenForceEnabledThisFrame)
    {
        g_BombQuickthrowEnabler_isNeedToFixRightArm = true;
    }
}
void WhenCombatActionsAreUpdatedChecksBombDrop_DisableOriginalBombDropHandlingInCombat(AllRegisters* params)
{
    params->GetRAX() = 0;
}
void WhenOnWallCheckingIfAssassinateAttemptTargetAvailable_DisableIfBombJustDropped(AllRegisters* params)
{
    if (g_BombQuickthrowEnabler_isStartedBombDropThisFrame)
    {
        params->rbx_ = 1;
    }
}
MoreSituationsToDropBomb::MoreSituationsToDropBomb()
{
    auto WhenBuildingArrayOfStatesToBeUpdated_AddQuickdropChecker = [&]() {
        uintptr_t whenFillingArrayOfStateEnumsToUpdate = 0x142656F47;
        PresetScript_CCodeInTheMiddle(whenFillingArrayOfStateEnumsToUpdate, 5,
            WhenFillingArrayOfStateEnumsToUpdate_EnableChecksForBombQuickDrop, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };
    auto StrangeFixToUnbreakCombatAndAssassinations = [&]() {
        uintptr_t someStrangeCallThatCausesCombatToBeBrokenIfQuickthrowIsEnabled = 0x14265DDAD;
        PresetScript_CCodeInTheMiddle(someStrangeCallThatCausesCombatToBeBrokenIfQuickthrowIsEnabled, 5,
            WhenUpdatingPlayerStatesMakingSomeStrangeCall_DoMagicToAllowQuickthrowAndAvoidTPose, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    };
    auto StrangeFixForTheStrangeFixToAvoidTPose = [&]() {
        uintptr_t whenLevelReloaded_somehowAssociatedWithTPose = 0x141A03880;
        PresetScript_CCodeInTheMiddle(whenLevelReloaded_somehowAssociatedWithTPose, 7,
            WhenLevelReloaded_MakePreparationsToAvoidTPose, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };

    WhenBuildingArrayOfStatesToBeUpdated_AddQuickdropChecker();
    StrangeFixToUnbreakCombatAndAssassinations();
    StrangeFixForTheStrangeFixToAvoidTPose();

    // Also allow during a wallrun and when standing in the V-shape of a tree or flagpole.
    uintptr_t whenCheckingIfShouldDisallowDropBombInWallrunAndTrees = 0x141AA7C51;
    PresetScript_CCodeInTheMiddle(whenCheckingIfShouldDisallowDropBombInWallrunAndTrees, 5,
        WhenCheckingIfShouldDisallowDropBombInWallrunAndTrees_DisobeyOrders, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);

    // Also allow during movement on steep slopes (like Notre Dame's).
    // This is actually independent from the above hacks.
    uintptr_t whenCheckingIfShouldDisallowDropBombOnSlopes = 0x142667042;
    PresetScript_CCodeInTheMiddle(whenCheckingIfShouldDisallowDropBombOnSlopes, 6,
        WhenCheckingIfShouldDisallowDropBombOnSlopes_DisobeyOrders, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);

    // Also allow during social stealth interaction like sitting on a bench, leaning on a wall.
    uintptr_t whenCheckingIfDropbombIsAvailableInAbilitySets = 0x142664422;
    PresetScript_NOP(whenCheckingIfDropbombIsAvailableInAbilitySets, 6);


    uintptr_t whenSuccessfullyStartingToDropBomb = 0x141A2C040;
    PresetScript_CCodeInTheMiddle(whenSuccessfullyStartingToDropBomb, 6,
        WhenSuccessfullyStartingToDropBomb_RememberSuccess, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenDropBombAnimationEnds = 0x141AA7DF0;
    PresetScript_CCodeInTheMiddle(whenDropBombAnimationEnds, 5,
        WhenDropBombAnimationEnds_ClearBombDropFlags, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    uintptr_t whenDropBombCheckerFinishes = 0x14265764C;
    uintptr_t whenDropBombCheckerFinishes_returnTo = 0x1426577BD;
    PresetScript_CCodeInTheMiddle(whenDropBombCheckerFinishes, 5,
        WhenDropBombCheckerFinishes_PretendArnoIsInHighProfileIfItsNeededToFixAnimations, whenDropBombCheckerFinishes_returnTo, false);

    uintptr_t whenHighProfileMovementIsDecided_insideGetter = 0x1409D9700;
    PresetScript_CCodeInTheMiddle(whenHighProfileMovementIsDecided_insideGetter, 5,
        WhenHighProfileMovementIsDecided_insideGetter_PretendArnoIsInHighProfileIfItsNeededToFixAnimations, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    auto AlsoAllowBombDropWhenHangingOnWallAndTheresAssassinationTargetAvailableBelow = [&]()
    {
        uintptr_t whenOnWallCheckingIfAssassinateAttemptTargetAvailable = 0x142651B03;
        PresetScript_CCodeInTheMiddle(whenOnWallCheckingIfAssassinateAttemptTargetAvailable, 7,
            WhenOnWallCheckingIfAssassinateAttemptTargetAvailable_DisableIfBombJustDropped, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };
    AlsoAllowBombDropWhenHangingOnWallAndTheresAssassinationTargetAvailableBelow();

    uintptr_t whenStatesUpdaterFinishes = 0x1426572B1;
    PresetScript_CCodeInTheMiddle(whenStatesUpdaterFinishes, 7,
        WhenStatesUpdaterFinishes_ClearFlagsForFrame, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    auto PreventDoubleBombDropInCombat = [&]()
    {
        uintptr_t whenCombatActionsAreUpdatedChecksBombDrop = 0x14265C72B;
        PresetScript_CCodeInTheMiddle(whenCombatActionsAreUpdatedChecksBombDrop, 5,
            WhenCombatActionsAreUpdatedChecksBombDrop_DisableOriginalBombDropHandlingInCombat, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    };
    //PreventDoubleBombDropInCombat();
}

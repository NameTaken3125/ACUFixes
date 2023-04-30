#include "pch.h"

#include "Hack_MoreResponsiveBombQuickDrop.h"







#include "ACU_InputUtils.h"
void WhenCheckingIfBombShouldBeQuickDropped_ForceIfSprinting(AllRegisters* params)
{
    bool doBombDrop = params->r15_ & 0xFF;
    if (doBombDrop)
        return;
    const bool forceBombDrop
        = ACU::Input::IsPressed(ActionKeyCode::BombDrop)
        && ACU::Input::IsPressed(ActionKeyCode::Sprint);
    params->r15_ = forceBombDrop;
}
LessFinickyBombQuickDrop::LessFinickyBombQuickDrop()
{
    uintptr_t whenCheckingIfBombShouldBeQuickDropped = 0x142666F8F;
    PresetScript_CCodeInTheMiddle(whenCheckingIfBombShouldBeQuickDropped, 7,
        WhenCheckingIfBombShouldBeQuickDropped_ForceIfSprinting, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}








#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"
#include "ACU_DefineNativeFunction.h"
DEFINE_GAME_FUNCTION(SmallArray_DWORD__AppendIfNotPresent_mb, 0x140B8E8F0, void, __fastcall, (SmallArray<uint32>* arr, uint32* newElement));
void AddBombDropStateToListOfToBeUpdated(SmallArray<uint32>& arrayToFill)
{
    uint32 stateEnumThatChecksForBombDrop = 1;
    if (std::find(arrayToFill.begin(), arrayToFill.end(), stateEnumThatChecksForBombDrop) != arrayToFill.end())
        return;
    SmallArray_DWORD__AppendIfNotPresent_mb(&arrayToFill, &stateEnumThatChecksForBombDrop);
}
void WhenFillingArrayOfStateEnumsToUpdate_EnableChecksForBombQuickDrop_1(AllRegisters* params)
{
    auto* arrayToFill = (SmallArray<uint32>*)params->rbx_;
    AddBombDropStateToListOfToBeUpdated(*arrayToFill);
}
void WhenFillingArrayOfStateEnumsToUpdate_EnableChecksForBombQuickDrop(AllRegisters* params)
{
    auto* arrayToFill = (SmallArray<uint32>*)(params->GetRSP() + 0x30);
    if (arrayToFill->size)
    {
        // _While_entering_ the Hidespot closet, the array is empty. Adding the one element crashes the game.
        AddBombDropStateToListOfToBeUpdated(*arrayToFill);
    }
}


bool g_BombQuickthrowEnabler_isLevelJustReloaded = false;
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
    *params->rax_ = strangeFunctionResult;
}
MoreSituationsToDropBomb::MoreSituationsToDropBomb()
{
    auto WhenBuildingArrayOfStatesToBeUpdated_AddQuickdropChecker = [&]() {
        uintptr_t whenFillingArrayOfStateEnumsToUpdate = 0x142656F4C;
        PresetScript_CCodeInTheMiddle(whenFillingArrayOfStateEnumsToUpdate, 6,
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
    uintptr_t whenCheckingIfShouldDisallowDropBomb = 0x141AA7C51;
    PresetScript_NOP(whenCheckingIfShouldDisallowDropBomb, 2);

    // Also allow during movement on steep slopes (like Notre Dame's).
    // This is actually independent from the above hacks.
    uintptr_t whenCheckingIfShouldDisallowDropBombOnSlopes = 0x142667042;
    PresetScript_NOP(whenCheckingIfShouldDisallowDropBombOnSlopes, 6);
}

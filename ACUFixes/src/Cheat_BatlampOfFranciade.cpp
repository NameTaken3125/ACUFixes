#include "pch.h"

#include "MainConfig.h"

#include "Cheat_BatlampOfFranciade.h"

#include "ACU/InventoryItemSettings.h"
#include "ACU/AvatarGearManager.h"
constexpr uint64 magicalLanternHandle = 0x000000238FD23D0F;
constexpr uint64 oldAndBustedLanternHandle = 0x000000238FD23D13;
AvatarGear* FindMagicalLanternGear(AvatarGearManager& agm)
{
    for (auto rit = agm.gears_1C_hasLanternAtEnd.rbegin(), rend = agm.gears_1C_hasLanternAtEnd.rend(); rit != rend; rit++)
    {
        if ((*rit)->inventoryItemSettings->handle == magicalLanternHandle)
        {
            return *rit;
        }
    }
    return nullptr;
}
void WhenOutOfMissionEquippingTheLantern_SubstituteNormalLanternWithTheMagicalOne(AllRegisters* params)
{
    if (!g_Config.cheats->batlampOfFranciadeManipulations->doReplaceNormalLampWithMagical)
    {
        return;
    }
    AvatarGearManager* agm = *(AvatarGearManager**)params->rbx_;
    AvatarGear*& gearToBeEquipped = (AvatarGear*&)*params->rax_;
    if (gearToBeEquipped->inventoryItemSettings->handle == oldAndBustedLanternHandle)
    {
        AvatarGear* magicalLanternGear = FindMagicalLanternGear(*agm);
        if (magicalLanternGear)
        {
            gearToBeEquipped = magicalLanternGear;
        }
    }
}
#include "ACU/LanterndlcComponent.h"
#include "ACU/Entity.h"
#include "ACU/AssassinAbilitySet.h"
#include "ACU/ActionAbilitySet.h"
#include "ACU/DominoComponent.h"
#include "ACU/DominoScriptExecutionContext.h"
#include "ACU/AIActionEntityOperatorData.h"
#include "ACU/ActionAbilitySet.h"
#include "ACU/PlayerProgressionManager.h"
#include "ACU/AssassinAbilitySet.h"

ActionAbilitySet* FindAbilitySetActionInTheOperatorDatas(SmallArray<DominoOperatorData*>& arrOperatorData)
{
    constexpr uintptr_t AIActionEntityOperatorData_VTBL = 0x142FE7BF0;
    constexpr uintptr_t ActionAbilitySet_VTBL = 0x142F95D10;
    for (DominoOperatorData* operatorData : arrOperatorData)
    {
        uintptr_t currentOpDataVTBL = *(uintptr_t*)operatorData;
        if (currentOpDataVTBL != AIActionEntityOperatorData_VTBL)
        {
            continue;
        }
        AIActionEntityOperatorData* aiActionOpData = static_cast<AIActionEntityOperatorData*>(operatorData);
        AIAction* aiAction = aiActionOpData->aiAction;
        if (!aiAction)
        {
            continue;
        }
        uintptr_t currentActionVTBL = *(uintptr_t*)aiAction;
        if (currentActionVTBL != ActionAbilitySet_VTBL)
        {
            continue;
        }
        return static_cast<ActionAbilitySet*>(aiAction);
    }
    return nullptr;
}
AssassinAbilitySet* FindAssassinAbilitySetUsedForTheLanternSequence(Entity& lanternEntity)
{
    constexpr uintptr_t dominoCpntVTBL = 0x1430640C0;
    Component* foundDominoCpnt = lanternEntity.FindComponentByVTBL(dominoCpntVTBL);
    if (!foundDominoCpnt) { return nullptr; }
    DominoComponent* domCpnt = static_cast<DominoComponent*>(foundDominoCpnt);
    DominoScriptExecutionContext* scriptExecCtx = domCpnt->aiScriptInstance_28.dominoScriptExecutionContext;
    if (!scriptExecCtx) { return nullptr; }

    ActionAbilitySet* foundActionAbilitySet = FindAbilitySetActionInTheOperatorDatas(scriptExecCtx->arrOperatorData);
    if (!foundActionAbilitySet) { return nullptr; }

    AssassinAbilitySet* aas = foundActionAbilitySet->shared_AbilitySet->GetPtr();
    return aas;
}
AssassinAbilitySet* FindAssassinAbilitySetUsedForTheLanternSequence(LanterndlcComponent& lantern)
{
    Entity* lanternEntity = lantern.entity;
    return FindAssassinAbilitySetUsedForTheLanternSequence(*lanternEntity);
}
AssassinAbilitySets* GetAbilitySets()
{
    return &PlayerProgressionManager::GetSingleton()->assassinAbilitySets;
}
using AbilitySetRawBytes_t = std::array<uint64, 5>;
static_assert(sizeof(AssassinAbilitySet::Flags) == sizeof(AbilitySetRawBytes_t));
AssassinAbilitySet::Flags GetAbilityFlagsForLanternSequence()
{
    constexpr AbilitySetRawBytes_t abilitySetDuringLanternSequence_bytes = { 0x0001000100000008, 0x0000000FBFFFFFFF, 0xDED3FFFC226FFFFF, 0x0FEFFFFFFFFFFFFF, 0x017F03090000FFFF };
    return (AssassinAbilitySet::Flags&)abilitySetDuringLanternSequence_bytes;
}
void ToggleMovementLock(AssassinAbilitySet& abilitySet, bool doUnlock)
{
    if (!doUnlock)
    {
        //AssassinAbilitySet::Flags abilitySetDuringLanternSequence = GetAbilityFlagsForLanternSequence();
        //abilitySet.flags = abilitySetDuringLanternSequence;
        return;
    }
    // Do Unlock movement
    AssassinAbilitySets* ass = GetAbilitySets();
    if (!ass) { return; }
    AssassinAbilitySet* mainAS = ass->GetLowestPrioritySet();
    if (!mainAS) { return; }
    abilitySet.flags = mainAS->flags;
}
bool IsAbilitySetsSame(const AssassinAbilitySet::Flags& lhs, const AssassinAbilitySet::Flags& rhs)
{
    return (AbilitySetRawBytes_t&)lhs == (AbilitySetRawBytes_t&)rhs;
}
bool IsMovementLimitedLikeWhenUsingTheBatlamp(AssassinAbilitySet& abilitySet)
{
    return IsAbilitySetsSame(abilitySet.flags, GetAbilityFlagsForLanternSequence());
}
void UnlockMovementForTheBatlamp(LanterndlcComponent& lanternCpnt)
{
    AssassinAbilitySet* aas = FindAssassinAbilitySetUsedForTheLanternSequence(lanternCpnt);
    if (!aas)
    {
        return;
    }
    ToggleMovementLock(*aas, true);
}
#include "ACU_InputUtils.h"

void OnLanternComponentUpdate(LanterndlcComponent& lanternCpnt)
{
    if (lanternCpnt.isInModeAbleToCharge_300)
    {
        constexpr float howLongItTakesToToggleLanternNormally = 0.5f;
        if (ACU::Input::IsJustPressedLong(ActionKeyCode::Reload, howLongItTakesToToggleLanternNormally))
        {
            lanternCpnt.isInModeAbleToCharge_300 = false;
            // Normally, when in can-charge mode, it's impossible to hide the lamp.
            // I'd like for the long-press-Reload to do the following:
            // - exit the can-charge mode (byte_300 = false)
            // - snuff out the lamp (byte_470 = false)
            // However, if I set `byte_470 = false` _here_, then, after returning from the hook,
            // The game's code will test again for the long-press-Reload, will succeed (as it's done in the same frame),
            // and flip `byte_470` back to `true`.
            // But for the same reason, if I _don't_ set `byte_470 = false` here,
            // then the game's code will do it on its own.

            //lanternCpnt.isAlight_mb = false;
        }
    }
    else
    {
        const bool doToggleChargeMode = ACU::Input::IsJustPressed(g_Config.cheats->batlampOfFranciadeManipulations->batlampChargeModeButton);
        if (doToggleChargeMode)
        {
            lanternCpnt.isInModeAbleToCharge_300 = !lanternCpnt.isInModeAbleToCharge_300;
        }
    }
    if (g_Config.cheats->batlampOfFranciadeManipulations->doUnlockMovementWithTheBatlamp)
    {
        UnlockMovementForTheBatlamp(lanternCpnt);
    }
}
void WhenLanternCpntIsUpdated_ManageLanternModes(AllRegisters* params)
{
    LanterndlcComponent& lanternCpnt = *(LanterndlcComponent*)*params->rax_;
    OnLanternComponentUpdate(lanternCpnt);
}
BatlampOfFrancide::BatlampOfFrancide()
{
    uintptr_t whenOutOfMissionEquippingTheLantern = 0x140D2B330;
    PresetScript_CCodeInTheMiddle(
        whenOutOfMissionEquippingTheLantern, 6,
        WhenOutOfMissionEquippingTheLantern_SubstituteNormalLanternWithTheMagicalOne, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenLanternCpntIsUpdated = 0x142663532;
    PresetScript_CCodeInTheMiddle(
        whenLanternCpntIsUpdated, 5,
        WhenLanternCpntIsUpdated_ManageLanternModes, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}
#include "ACU/CLAssassin.h"
LanterndlcComponent* GetLanternComponent()
{
    CLAssassin* clAssassin = CLAssassin::GetSingleton();
    if (!clAssassin) { return nullptr; }
    auto* hasLanternCpnt = clAssassin->hasLanternCpnt;
    if (!hasLanternCpnt) { return nullptr; }
    return hasLanternCpnt->lanternCpnt;
}
#include "ImGuiCTX.h"
void DrawBatlampControls()
{
    ImGuiCTX::Indent _ind;
    ImGui::Checkbox("Replace the normal lantern with the magical one",
        &g_Config.cheats->batlampOfFranciadeManipulations->doReplaceNormalLampWithMagical.get());
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "On the next level reload the regular lamp will be replaced by the Head of Saint Denis\n"
            "as it is used in the final mission of the Dead Kings DLC."
        );
    }
    ImGui::Checkbox("Unlock movement when using the magical lamp",
        &g_Config.cheats->batlampOfFranciadeManipulations->doUnlockMovementWithTheBatlamp.get());
    LanterndlcComponent* lanternCpnt = GetLanternComponent();
    if (!lanternCpnt)
    {
        ImGui::BulletText("Couldn't find a `LanterndlcComponent`");
        return;
    }
    ImGui::Checkbox("Allow to charge now", (bool*)&lanternCpnt->isInModeAbleToCharge_300);
}

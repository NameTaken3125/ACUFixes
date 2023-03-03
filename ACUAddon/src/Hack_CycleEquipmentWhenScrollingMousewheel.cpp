#include "pch.h"

#include "Hack_CycleEquipmentWhenScrollingMousewheel.h"

#include "ACU/InputContainer.h"
#include "ACU/World.h"
#include "ACU/UIWorldComponent.h"
#include "ACU/HUDLetterReaderModule.h"
#include "ACU/HUDQuickSelectModule.h"
HUDLetterReaderModule* FindHUDLetterReaderModule(UIWorldComponent& uiWorldCpnt)
{
    using vtbl_t = uint64;
    constexpr vtbl_t hudLetterReaderModuleVTBL = 0x142ED07C0;
    for (HUDModule* hudModule : uiWorldCpnt.hudModules)
    {
        vtbl_t hudModuleVtbl = *(vtbl_t*)hudModule;
        bool isLetterReader = hudModuleVtbl == hudLetterReaderModuleVTBL;
        if (isLetterReader)
        {
            return static_cast<HUDLetterReaderModule*>(hudModule);
        }
    }
    return nullptr;
}
HUDQuickSelectModule* FindHUDQuickSelectModule(UIWorldComponent& uiWorldCpnt)
{
    using vtbl_t = uint64;
    constexpr vtbl_t hudQuickSelectModuleVTBL = 0x142ED8B70;
    for (HUDModule* hudModule : uiWorldCpnt.hudModules)
    {
        vtbl_t hudModuleVtbl = *(vtbl_t*)hudModule;
        bool isLetterReader = hudModuleVtbl == hudQuickSelectModuleVTBL;
        if (isLetterReader)
        {
            return static_cast<HUDQuickSelectModule*>(hudModule);
        }
    }
    return nullptr;
}
#include "ACU/Enum_EquipmentType.h"
std::optional<ActionKeyCode> GetActionKeyCodeForEquipmentType(EquipmentType equipType)
{
    switch (equipType)
    {
    case EquipmentType::Pistol:
    case EquipmentType::Rifle:
    case EquipmentType::Blunderbuss:
    case EquipmentType::GuillotineGun:
        return ActionKeyCode::SlotGun;
        break;
    case EquipmentType::SmokeBomb:
        return ActionKeyCode::SlotSmokeBomb;
        break;
    case EquipmentType::MoneyPouch:
        return ActionKeyCode::SlotMoneyPouch;
        break;
    case EquipmentType::PoisonBomb:
        return ActionKeyCode::SlotPoisonBomb;
        break;
    case EquipmentType::StunBomb:
        return ActionKeyCode::SlotStunBomb;
        break;
    case EquipmentType::PhantomBlade:
        return ActionKeyCode::SlotPhantom;
        break;
    case EquipmentType::BerserkBlade:
        return ActionKeyCode::SlotBerserk;
        break;
    case EquipmentType::CherryBomb:
        return ActionKeyCode::SlotCherryBomb;
        break;
    case EquipmentType::Slot9Booster:
        return ActionKeyCode::Slot9_booster;
        break;
    default:
        break;
    }
    return {};
}

constexpr inline std::array<EquipmentType, 6> g_BombTypes = {
    EquipmentType::SmokeBomb,
    EquipmentType::StunBomb,
    EquipmentType::CherryBomb,
    EquipmentType::PoisonBomb,
    EquipmentType::MoneyPouch,
    EquipmentType::Slot9Booster,
};
constexpr std::optional<int> GetEquipmentTypeIndexInCycle_Bombs(EquipmentType equipmentType)
{
    switch (equipmentType)
    {
    case EquipmentType::SmokeBomb:
        return 0;
        break;
    case EquipmentType::StunBomb:
        return 1;
        break;
    case EquipmentType::CherryBomb:
        return 2;
        break;
    case EquipmentType::PoisonBomb:
        return 3;
        break;
    case EquipmentType::MoneyPouch:
        return 4;
        break;
    case EquipmentType::Slot9Booster:
        return 5;
        break;
    default:
        break;
    }
    return {};
}
constexpr bool IsBomb(EquipmentType equipType)
{
    switch (equipType)
    {
    case EquipmentType::SmokeBomb:
    case EquipmentType::StunBomb:
    case EquipmentType::CherryBomb:
    case EquipmentType::PoisonBomb:
    case EquipmentType::MoneyPouch:
    case EquipmentType::Slot9Booster:
        return true;
    default:
        break;
    }
    return false;
}

#include "ACU/BhvAssassin.h"
#include "ACU/CharacterAI.h"
#include "ACU/CSrvPlayerWeaponSwitch.h"
CSrvPlayerWeaponSwitch* FindPlayerWeaponSwitch()
{
    BhvAssassin* bhvAssassin = BhvAssassin::GetSingletonPlayer();
    if (!bhvAssassin) { return nullptr; }
    CharacterAI* charAI = bhvAssassin->characterAI_toHealthAndInventory;
    if (!charAI) { return nullptr; }

    using vtbl_t = uint64;
    constexpr vtbl_t CSrvPlayerWeaponSwitch_VTBL = 0x142F59810;
    for (Statechart* stc : charAI->childStatecharts_mb_toHealth_60)
    {
        if (!stc) { continue; }
        vtbl_t currentVtbl = *(vtbl_t*)stc;
        if (currentVtbl == CSrvPlayerWeaponSwitch_VTBL)
        {
            return static_cast<CSrvPlayerWeaponSwitch*>(stc);
        }
    }
    return nullptr;
}
// Gets specifically what is equipped, and so ignores locked quick-select slots
// like poison gas that unlocks later in game.
EquipmentType GetCurrentEquipmentType_Bombs()
{
    CSrvPlayerWeaponSwitch* weaponSwitcher = FindPlayerWeaponSwitch();
    if (!weaponSwitcher) { return EquipmentType::SmokeBomb; }
    return weaponSwitcher->equipType_bombs;
}
EquipmentType g_LastQuickSelectedEquipment = EquipmentType::SmokeBomb;
EquipmentType GetCurrentEquipmentSelection_Bombs()
{
    if (IsBomb(g_LastQuickSelectedEquipment))
    {
        return g_LastQuickSelectedEquipment;
    }
    return GetCurrentEquipmentType_Bombs();
}
int ProgressIndexInCycle(int currentIdx, int stepHowManyFromCurrent, size_t cycleSize)
{
    int result = (currentIdx + stepHowManyFromCurrent) % (int)cycleSize;
    if (result < 0) { result += (int)cycleSize; }
    return result;
}
#include "ACU/HUDQuickSelectComponent.h"
#include "ACU/QuickSelectButtonComponent.h"
#include "ACU/SharedPtr.h"
#include "ACU/Entity.h"
HUDQuickSelectComponent* FindHUDQuickSelectComponent(HUDQuickSelectModule& qsModule)
{
    if (qsModule.p_84.size <= 0) { return nullptr; }
    Entity* qsCpntHolder = qsModule.p_84[0]->GetPtr();
    if (!qsCpntHolder) { return nullptr; }
    constexpr uint64 qsCpntVTBL = 0x142ED86D0;
    Component* qsCpnt = qsCpntHolder->FindComponentByVTBL(qsCpntVTBL);
    if (!qsCpnt) { return nullptr; }
    return static_cast<HUDQuickSelectComponent*>(qsCpnt);
}
// Return the slot index for _currently_highlighted_ Bomb-type equipment.
// Checks if Weapon Selector is opened, and whether the last selected item is of
// Bomb type.
static std::optional<int> GetCurrentHighlightedSlot_Bombs()
{
    World* world = World::GetSingleton();
    if (!world) { return {}; }
    HUDQuickSelectModule* hudQuickSelect = FindHUDQuickSelectModule(*world->uiWorldComponent);
    if (!hudQuickSelect) { return {}; }

    HUDQuickSelectComponent* qsCpnt = FindHUDQuickSelectComponent(*hudQuickSelect);
    if (!qsCpnt) { return {}; }
    if (!qsCpnt->isVisible_d3) { return {}; }
    //QuickSelectButtonComponent* lastHighlightedQSButton = qsCpnt->quickSelectButtonComponent_188_pendingSelection;
    //if (!lastHighlightedQSButton) { return {}; }
    //EquipmentType lastHighlightedEquipment = lastHighlightedQSButton->equipmentType;

    EquipmentType lastHighlightedEquipment = g_LastQuickSelectedEquipment;
    std::optional<int> bombSlot = GetEquipmentTypeIndexInCycle_Bombs(lastHighlightedEquipment);
    return bombSlot;
}
static int GetCurrentEquipmentSlot_Bombs()
{
    EquipmentType currentBombType = GetCurrentEquipmentSelection_Bombs();
    std::optional<int> correspondingBombSlot = GetEquipmentTypeIndexInCycle_Bombs(currentBombType);
    return correspondingBombSlot ? *correspondingBombSlot : 0;
}
// If the weapon selector is closed or if the currently highlighted item isn't a bomb,
// select the currently equipped bomb type.
// Otherwise, cycle bombs forward or backward.
static void InjectEquipmentSelection_CycleBombs(KeyStates& keyStatesThisFrame, bool isDirectionForward)
{
    int slotToInject;
    std::optional<int> currentHighlightedSlot = GetCurrentHighlightedSlot_Bombs();
    if (!currentHighlightedSlot)
    {
        int currentEquippedSlot = GetCurrentEquipmentSlot_Bombs();
        slotToInject = currentEquippedSlot;
    }
    else
    {
        const int howManySlotsFromCurrent = isDirectionForward ? 1 : -1;
        slotToInject = ProgressIndexInCycle(*currentHighlightedSlot, howManySlotsFromCurrent, g_BombTypes.size());
    }
    ActionKeyCode actionToInject = GetActionKeyCodeForEquipmentType(g_BombTypes[slotToInject]).value();
    uint8& isNewSlotMarkedAsPressed = ((std::array<uint8, (size_t)ActionKeyCode::ACTION_SET_SIZE>&)keyStatesThisFrame)[(size_t)actionToInject];
    isNewSlotMarkedAsPressed = true;
}
static void InjectEquipmentSelection_CycleBombsForward(KeyStates& keyStatesThisFrame)
{
    InjectEquipmentSelection_CycleBombs(keyStatesThisFrame, true);
}
static void InjectEquipmentSelection_CycleBombsBack(KeyStates& keyStatesThisFrame)
{
    InjectEquipmentSelection_CycleBombs(keyStatesThisFrame, false);
}
void CycleEquipmentByScrollingMousewheel_BombsOnly(AllRegisters* params)
{
    World* world = World::GetSingleton();
    if (!world) { return; }
    HUDLetterReaderModule* letterReader = FindHUDLetterReaderModule(*world->uiWorldComponent);
    if (!letterReader) { return; }
    const bool isReadingLetterNow = letterReader->isVisible_158;
    if (isReadingLetterNow) { return; }

    InputContainerBig* inpCont = (InputContainerBig*)params->rbx_;
    const bool isMWScrollUp = inpCont->IsMouseWheelScrollingUp();
    const bool isMWScrollDown = inpCont->IsMouseWheelScrollingDown();

    if (isMWScrollUp)
        InjectEquipmentSelection_CycleBombsBack(inpCont->keyStates_thisFrame);
    if (isMWScrollDown)
        InjectEquipmentSelection_CycleBombsForward(inpCont->keyStates_thisFrame);
}
void WhenAttemptedToQuickSelectEquipment_SaveSelection(AllRegisters* params)
{
    // The equipment might be locked, that's why it's "attempted".
    EquipmentType attemptedSelection = (EquipmentType&)*params->rax_;
    g_LastQuickSelectedEquipment = attemptedSelection;
}
InputInjection_CycleEquipmentWhenScrollingMousewheel::InputInjection_CycleEquipmentWhenScrollingMousewheel()
{
    constexpr uintptr_t whenCheckingKeymapForFrame = 0x14273BC75;
    PresetScript_CCodeInTheMiddle(
        whenCheckingKeymapForFrame, 6
        , CycleEquipmentByScrollingMousewheel_BombsOnly
        , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
        , true);
    constexpr uintptr_t whenAttemptedToQuickSelectEquipment = 0x14082EB9A;
    PresetScript_CCodeInTheMiddle(whenAttemptedToQuickSelectEquipment, 5
        , WhenAttemptedToQuickSelectEquipment_SaveSelection, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

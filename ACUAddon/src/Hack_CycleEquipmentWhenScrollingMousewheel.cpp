#include "pch.h"

#include "Hack_CycleEquipmentWhenScrollingMousewheel.h"

#include "ACU/InputContainer.h"
void InjectNumpad46WhenScrollingMouseWheel_old(AllRegisters* params)
{
    InputContainerBig* inpCont = (InputContainerBig*)params->rbx_;
    uint8& isMarkedAsPressed_CycleLeftHand = inpCont->keyStates_thisFrame.isPressed_numpad4cycleLeftHand;
    uint8& isMarkedAsPressed_CycleBombs = inpCont->keyStates_thisFrame.isPressed_numpad6cycleBombs;
    if (!isMarkedAsPressed_CycleLeftHand)
    {
        isMarkedAsPressed_CycleLeftHand = inpCont->IsMouseWheelScrollingUp();
    }
    if (!isMarkedAsPressed_CycleBombs)
    {
        isMarkedAsPressed_CycleBombs = inpCont->IsMouseWheelScrollingDown();
    }
}
#include "ACU/World.h"
#include "ACU/UIWorldComponent.h"
#include "ACU/HUDLetterReaderModule.h"

HUDLetterReaderModule* FindHUDLetterReaderModule(UIWorldComponent& uiWorldCpnt)
{
    using vtbl_t = uint64;
    constexpr vtbl_t hudLetterReaderModuleVTBL = 0x142ED07C0;
    for (HUDModule* hudModule : uiWorldCpnt.hudModules)
    {
        vtbl_t hudModuleVtbl = *(vtbl_t*)hudModule;
        bool isLetterReader = hudModuleVtbl == 0x142ED07C0;
        if (isLetterReader)
        {
            return static_cast<HUDLetterReaderModule*>(hudModule);
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
constexpr int GetEquipmentTypeIndexInCycle_Bombs(EquipmentType equipmentType)
{
    switch (equipmentType)
    {
    case SmokeBomb:
        return 0;
        break;
    case StunBomb:
        return 1;
        break;
    case CherryBomb:
        return 2;
        break;
    case PoisonBomb:
        return 3;
        break;
    case MoneyPouch:
        return 4;
        break;
    case Slot9Booster:
        return 5;
        break;
    default:
        break;
    }
    return 0;
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
EquipmentType GetCurrentEquipmentType_Bombs()
{
    CSrvPlayerWeaponSwitch* weaponSwitcher = FindPlayerWeaponSwitch();
    if (!weaponSwitcher) { return EquipmentType::SmokeBomb; }
    return weaponSwitcher->equipType_bombs;
}
int ProgressIndexInCycle(int currentIdx, int stepHowManyFromCurrent, size_t cycleSize)
{
    int result = (currentIdx + stepHowManyFromCurrent) % (int)cycleSize;
    if (result < 0) { result += (int)cycleSize; }
    return result;
}
EquipmentType ProgressEquipmentCycle_Bombs(int stepHowManyFromCurrentEquipment)
{
    EquipmentType currentBomb = GetCurrentEquipmentType_Bombs();
    int currentBombSlotIdx = GetEquipmentTypeIndexInCycle_Bombs(currentBomb);
    int newBombSlotIdx = ProgressIndexInCycle(currentBombSlotIdx, stepHowManyFromCurrentEquipment, g_BombTypes.size());
    return g_BombTypes[newBombSlotIdx];
}
void InjectEquipmentSelection_ProgressFromCurrent(KeyStates& keyStatesThisFrame, int stepHowManyFromCurrentEquipment)
{
    EquipmentType progressedEquipmentCycle = ProgressEquipmentCycle_Bombs(stepHowManyFromCurrentEquipment);
    std::optional<ActionKeyCode> progressedEquipmentSlot = GetActionKeyCodeForEquipmentType(progressedEquipmentCycle);
    if (!progressedEquipmentSlot) { return; }
    uint8& isNewSlotMarkedAsPressed = ((std::array<uint8, (size_t)ActionKeyCode::ACTION_SET_SIZE>&)keyStatesThisFrame)[(size_t)*progressedEquipmentSlot];
    isNewSlotMarkedAsPressed = true;
}
void InjectEquipmentSelection_PreviousFromCurrent(KeyStates& keyStatesThisFrame)
{
    InjectEquipmentSelection_ProgressFromCurrent(keyStatesThisFrame, -1);
}
void InjectEquipmentSelection_NextFromCurrent(KeyStates& keyStatesThisFrame)
{
    InjectEquipmentSelection_ProgressFromCurrent(keyStatesThisFrame, 1);
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
        InjectEquipmentSelection_PreviousFromCurrent(inpCont->keyStates_thisFrame);
    if (isMWScrollDown)
        InjectEquipmentSelection_NextFromCurrent(inpCont->keyStates_thisFrame);
}
InputInjection_CycleEquipmentWhenScrollingMousewheel::InputInjection_CycleEquipmentWhenScrollingMousewheel()
{
    constexpr uintptr_t whenCheckingKeymapForFrame = 0x14273BC75;
    PresetScript_CCodeInTheMiddle(
        whenCheckingKeymapForFrame, 6
        , CycleEquipmentByScrollingMousewheel_BombsOnly
        , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
        , true);
}

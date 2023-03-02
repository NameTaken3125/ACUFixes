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

    uint8& isMarkedAsPressed_CycleBombs = inpCont->keyStates_thisFrame.isPressed_numpad6cycleBombs;
    if (isMWScrollUp)
        isMarkedAsPressed_CycleBombs = true;
    if (isMWScrollDown)
        isMarkedAsPressed_CycleBombs = true;
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

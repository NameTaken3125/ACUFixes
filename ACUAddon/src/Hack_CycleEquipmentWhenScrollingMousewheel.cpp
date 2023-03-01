#include "pch.h"

#include "Hack_CycleEquipmentWhenScrollingMousewheel.h"

#include "ACU/InputContainer.h"
void InjectNumpad46WhenScrollingMouseWheel(AllRegisters* params)
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
InputInjection_CycleEquipmentWhenScrollingMousewheel::InputInjection_CycleEquipmentWhenScrollingMousewheel()
{
    constexpr uintptr_t whenCheckingKeymapForFrame = 0x14273BC75;
    PresetScript_CCodeInTheMiddle(
        whenCheckingKeymapForFrame, 6
        , InjectNumpad46WhenScrollingMouseWheel
        , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
        , true);
}

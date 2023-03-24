#include "pch.h"

#include "ACU/InputContainer.h"

#include "ACU_InputUtils.h"
#include "ACU_DefineNativeFunction.h"



DEFINE_GAME_FUNCTION(InputContainer__IsLongPressJustHappened, 0x1427202B0, char, __fastcall, (InputContainer* a1, ActionKeyCode p_actionKeyCode, float p_testHowLong, __int64 a4));

#include "base.h"
void GameRawInputHook_ImGuiLayer(InputContainerBig& inpCont)
{
    if (Base::Data::IsImGuiInitialized && Base::Data::ShowMenu)
    {
        std::memset(inpCont.isPressed_byScancode, 0, sizeof(inpCont.isPressed_byScancode));
        inpCont.mouseState.mouseDeltaIntForCamera_X = inpCont.mouseState.mouseDeltaIntForCamera_Y = 0;
    }
}

namespace ACU::Input {

class InputHooks
{
    std::array<uint8, 256> m_PrevFrameKeyStates;
    std::array<uint8, 256> m_ThisFrameKeyStates;

    void UpdateEvolvedKeyStates(InputContainerBig& inpCont)
    {
        constexpr size_t keyStatesArraySize = sizeof(m_PrevFrameKeyStates);
        static_assert(keyStatesArraySize == sizeof(m_ThisFrameKeyStates));
        static_assert(keyStatesArraySize == sizeof(InputContainerBig::isPressed_byScancode));
        std::memcpy(&m_PrevFrameKeyStates, &m_ThisFrameKeyStates, keyStatesArraySize);
        std::memcpy(&m_ThisFrameKeyStates, &inpCont.isPressed_byScancode, keyStatesArraySize);
    }
public:
    void OnRawInputUpdate(InputContainerBig& inpCont)
    {
        UpdateEvolvedKeyStates(inpCont);
        GameRawInputHook_ImGuiLayer(inpCont);
    }
    bool IsJustPressed(uint8 scancode)
    {
        return m_ThisFrameKeyStates[scancode] && !m_PrevFrameKeyStates[scancode];
    }
    static InputHooks& GetSingleton()
    {
        static InputHooks singleton;
        return singleton;
    }
};

InputContainerBig* Get_InputContainerBig()
{
    return HasInputContainers::GetSingleton()->p_10->inputContainerBig;
}
bool IsPressed(BindableKeyCode_Keyboard keycode)
{
    return ACU::Input::Get_InputContainerBig()->isPressed_byScancode[(uint32)keycode];
}
bool IsJustPressed(BindableKeyCode_Keyboard keycode)
{
    return InputHooks::GetSingleton().IsJustPressed((uint8)keycode);
}
bool IsJustPressedLong(ActionKeyCode actionKey, float howLong)
{
    return InputContainer__IsLongPressJustHappened(&InputContainer::GetMainSingleton(), actionKey, howLong, 0);
}


bool IsPressedRMB() { return InputContainer::GetMainSingleton().keyStates_thisFrame.isPressed_RMB; }

} // namespace ACU::Input

#include "Hack_GameRawInputHook.h"

void BeforeActionKeysAreUpdatedFromScancodes_RunRawInputHooks(AllRegisters* params)
{
    InputContainerBig& inpBig = *(InputContainerBig*)params->rbx_;
    ACU::Input::InputHooks::GetSingleton().OnRawInputUpdate(inpBig);
}
GameRawInputHook::GameRawInputHook()
{
    uintptr_t beforeActionKeysAreUpdatedFromScancodes = 0x14273BC64;
    PresetScript_CCodeInTheMiddle(
        beforeActionKeysAreUpdatedFromScancodes, 6,
        BeforeActionKeysAreUpdatedFromScancodes_RunRawInputHooks, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

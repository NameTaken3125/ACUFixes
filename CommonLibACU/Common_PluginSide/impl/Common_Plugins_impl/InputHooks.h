#pragma once

#include "ACU/InputContainer.h"
#include "ACU/Enum_MouseButtons.h"
#include "Common_Plugins/Enum_BindableKeyCode_Keyboard.h"

class PluginLoader_UpdateInputHooks;

namespace ACU::Input {

class InputHooks
{
    std::array<uint8, 256> m_PrevFrameKeyStates;
    std::array<uint8, 256> m_ThisFrameKeyStates;
    std::array<uint8, 5> m_PrevFrameMouseButtonStates;
    std::array<uint8, 5> m_ThisFrameMouseButtonStates;


private:
    friend PluginLoader_UpdateInputHooks;
    void UpdateEvolvedKeyStates(InputContainerBig& inpCont)
    {
        constexpr size_t keyStatesArraySize = sizeof(m_PrevFrameKeyStates);
        static_assert(keyStatesArraySize == sizeof(m_ThisFrameKeyStates));
        static_assert(keyStatesArraySize == sizeof(InputContainerBig::isPressed_byScancode));
        constexpr size_t mouseButtonStatesArraySize = sizeof(m_PrevFrameMouseButtonStates);
        static_assert(mouseButtonStatesArraySize == sizeof(m_ThisFrameMouseButtonStates));
        static_assert(mouseButtonStatesArraySize == sizeof(MouseState::mouseButtonStates));
        std::memcpy(&m_PrevFrameKeyStates, &m_ThisFrameKeyStates, keyStatesArraySize);
        std::memcpy(&m_ThisFrameKeyStates, &inpCont.isPressed_byScancode, keyStatesArraySize);
        std::memcpy(&m_PrevFrameMouseButtonStates, &m_ThisFrameMouseButtonStates, mouseButtonStatesArraySize);
        std::memcpy(&m_ThisFrameMouseButtonStates, &inpCont.mouseState.mouseButtonStates, mouseButtonStatesArraySize);
    }
public:
    bool IsJustPressed(uint8 scancode)
    {
        return m_ThisFrameKeyStates[scancode] && !m_PrevFrameKeyStates[scancode];
    }
    bool IsJustPressed(MouseButton mouseButton)
    {
        return m_ThisFrameMouseButtonStates[(int)mouseButton] && !m_PrevFrameMouseButtonStates[(int)mouseButton];
    }
    bool IsJustReleased(MouseButton mouseButton)
    {
        return !m_ThisFrameMouseButtonStates[(int)mouseButton] && m_PrevFrameMouseButtonStates[(int)mouseButton];
    }
    bool IsJustReleased(BindableKeyCode_Keyboard keycode)
    {
        return !m_ThisFrameKeyStates[(int)keycode] && m_PrevFrameKeyStates[(int)keycode];
    }
};
extern InputHooks* g_InputHooksSingletonPtr; // Pointer is stored in the plugin; points to the instance owned by the PluginLoader.
} // namespace ACU::Input

#include "pch.h"

#include "Hack_SlowMenacingWalkAndAutowalk.h"

#include <vmath/vmath.h>
#include "ACU/InputContainer.h"
class InjectedAutowalkManager
{
    float FindCurrentTime() { return GetTickCount64() / 1000.0f; }
    struct Mode_Clear {};
    struct Mode_AutowalkRequested
    {
        float m_BeginAutowalkRequest_timestamp;
        std::optional<Vector2f> m_LastSavedMovementVec;
        bool IsExpectationExpired(float currentTime)
        {
            constexpr float requestedModeMaxExpectation = 1.0f;
            return currentTime > m_BeginAutowalkRequest_timestamp + requestedModeMaxExpectation;
        }
    };
    struct Mode_AutowalkActive
    {
        Vector2f m_LastSavedMovementVec;
    };
    std::variant<
        Mode_Clear
        , Mode_AutowalkRequested
        , Mode_AutowalkActive
    > m_CurrentMode;
public:
    void OnPressedB()
    {
        if (auto* activeAutowalk = std::get_if<Mode_AutowalkActive>(&m_CurrentMode))
        {
            m_CurrentMode = Mode_Clear();
            return;
        }
        OnInitiateAutowalk();
    }
    void OnAdjustCurrentMovement(Vector2f& movementVecInOut)
    {
        if (auto* activeMode = std::get_if<Mode_AutowalkActive>(&m_CurrentMode))
        {
            if (movementVecInOut.lengthSq() != 0)
            {
                // Is manually overridden.
                m_CurrentMode = Mode_Clear();
                return;
            }
            movementVecInOut = activeMode->m_LastSavedMovementVec;
            return;
        }
        else if (auto* requestedNotYetActive = std::get_if<Mode_AutowalkRequested>(&m_CurrentMode))
        {
            const bool isCurrentVecZero = movementVecInOut.lengthSq() == 0;
            if (!isCurrentVecZero) { requestedNotYetActive->m_LastSavedMovementVec = movementVecInOut; }
            bool shouldEndExpectationMode = false;
            if (isCurrentVecZero)
            {
                if (requestedNotYetActive->m_LastSavedMovementVec)
                    // If the player isn't moving AND hasn't been moving since autowalk was requested,
                    // we DON'T stop the request mode.
                    // Maybe the player has pressed the Autowalk button, and THEN immediately presses one of WASD.
                    shouldEndExpectationMode = true;
            }
            if (!shouldEndExpectationMode) {
                shouldEndExpectationMode = requestedNotYetActive->IsExpectationExpired(FindCurrentTime());
            }
            if (shouldEndExpectationMode)
            {
                // End pending request. Whether or not autowalk becomes active depends on
                // whether any input has been recorded.
                if (!requestedNotYetActive->m_LastSavedMovementVec)
                {
                    m_CurrentMode = Mode_Clear();
                    return;
                }
                m_CurrentMode = Mode_AutowalkActive{ *requestedNotYetActive->m_LastSavedMovementVec };
                return;
            }
        }
    }
private:
    void OnInitiateAutowalk()
    {
        m_CurrentMode = Mode_AutowalkRequested{ FindCurrentTime() };
    }
};
#include "MainConfig.h"
#include "Enum_BindableKeyCode_Keyboard.h"
inline uint8 ScancodeFromMyKeycode(BindableKeyCode_Keyboard keycode)
{
    return (uint8)keycode;
}
void OnMovementVectorUpdate_ManageAutowalk(Vector2f& movementVecWithoutAdjustments, InputContainerBig* inpCont)
{
    static InjectedAutowalkManager autowalkManager;
    const uint8 scancodeB = ScancodeFromMyKeycode(autowalkButton);
    if (inpCont->isPressed_byScancode[scancodeB])
    {
        autowalkManager.OnPressedB();
    }
    autowalkManager.OnAdjustCurrentMovement(movementVecWithoutAdjustments);
}
bool IsInSlowWalkMode()
{
    const bool isCapsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    return isCapsLockOn;
}
constexpr float slowWalkMode_SpeedMultiplier = 0.5f;
void AttenuateMovementVector(AllRegisters* params)
{
    Vector2f& movementVector = (Vector2f&)*params->rax_;
    InputContainer* inpCont = (InputContainer*)params->rbx_;
    OnMovementVectorUpdate_ManageAutowalk(movementVector, inpCont->GetInputContainerBig());

    if (IsInSlowWalkMode())
    {
        movementVector *= slowWalkMode_SpeedMultiplier;
        int noop = 0;
    }
}
void WhenCheckingIfInputIsStrongEnoughToExitHaystack_IgnoreSlowWalk(AllRegisters* params)
{
    Vector2f& inputMovementVec = *(Vector2f*)params->r14_;
    params->XMM0.f0 = inputMovementVec.length();
    if (IsInSlowWalkMode())
    {
        params->XMM0.f0 /= (slowWalkMode_SpeedMultiplier * slowWalkMode_SpeedMultiplier);
    }
}
AllowSlowMenacingWalkAndAutowalk::AllowSlowMenacingWalkAndAutowalk()
{
    CCodeInTheMiddleFunctionPtr_t receiverFunc = &AttenuateMovementVector;
    uintptr_t whereToInject = 0x142739CBE;
    constexpr size_t howManyBytesStolen = 7;
    const bool isNeedToExecuteStolenBytesAfterwards = true;
    PresetScript_CCodeInTheMiddle(
        whereToInject, howManyBytesStolen
        , receiverFunc
        , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
        , isNeedToExecuteStolenBytesAfterwards);
    uintptr_t whenCheckingIfInputIsStrongEnoughToExitHaystack = 0x14192A6D1;
    uintptr_t whenCheckingIfInputIsStrongEnoughToExitHaystack_returnAfterCalculation = 0x14192A6EF;
    PresetScript_CCodeInTheMiddle(whenCheckingIfInputIsStrongEnoughToExitHaystack, 7,
        WhenCheckingIfInputIsStrongEnoughToExitHaystack_IgnoreSlowWalk, whenCheckingIfInputIsStrongEnoughToExitHaystack_returnAfterCalculation, false);
}

#include "pch.h"

#include "AutoAssemblerKinda.h"
#include "Hack_EnterWindowsWhenRisPressed.h"

// Async-constructing the AutoAssemblerWrapper<CodeHolderObject> is better, because all the VirtualAllocs
// produce a noticeable stutter on creation otherwise.
template<typename Ty>
class AsyncConstructed
{
public:
    Ty* m_CachedPtr = nullptr;
    std::optional<Ty> m_Instance;
    std::future<void> m_Future;
    std::optional<std::exception> m_Exception;
public:
    AsyncConstructed()
    {
        m_Future = std::async(std::launch::async, [&]() {
            try
            {
                m_Instance.emplace();
                m_CachedPtr = &m_Instance.value();
            }
            catch (const std::exception& e)
            {
                m_Exception = e;
            }
            });
    }
    Ty* get() { return m_CachedPtr; }
    std::exception* GetException()
    {
        return m_Exception ? &m_Exception.value() : nullptr;
    }
};

struct AllowSlowMenacingWalkAndAutowalk : AutoAssemblerCodeHolder_Base
{
    AllowSlowMenacingWalkAndAutowalk();
};
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
            constexpr float requestedModeMaxExpectation = 0.5f;
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
void OnMovementVectorUpdate_ManageAutowalk(Vector2f& movementVecWithoutAdjustments, InputContainerBig* inpCont)
{
    static InjectedAutowalkManager autowalkManager;
    constexpr int scancodeB = 48;
    if (inpCont->isPressed_byScancode[scancodeB])
    {
        autowalkManager.OnPressedB();
    }
    autowalkManager.OnAdjustCurrentMovement(movementVecWithoutAdjustments);
}
void AttenuateMovementVector(AllRegisters* params)
{
    Vector2f& movementVector = (Vector2f&)*params->rax_;
    InputContainer* inpCont = (InputContainer*)params->rbx_;
    OnMovementVectorUpdate_ManageAutowalk(movementVector, inpCont->GetInputContainerBig());
    const bool isCapsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    if (isCapsLockOn)
    {
        constexpr float slowWalkMode_SpeedMultiplier = 0.5f;
        movementVector *= slowWalkMode_SpeedMultiplier;
        int noop = 0;
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
}
template<typename floatlike>
floatlike simple_interp(floatlike mn, floatlike mx)
{
    auto now = GetTickCount64();
    float speed = 0.001f * 1.5f;
    float interp = sin(now * speed);
    interp = (interp + 1) / 2;
    return mn + (mx - mn) * interp;
}
void FOVGames(AllRegisters* params)
{
    params->XMM1.f0 = simple_interp(0.5f, 1.0f);
}
struct PlayWithFOV : AutoAssemblerCodeHolder_Base
{
    PlayWithFOV()
    {
        PresetScript_CCodeInTheMiddle(
            0x141F3FE3B, 6
            , FOVGames
            , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
            , true);
    }
};

#include "ACU/ACUGetSingletons.h"
#include "ACU/Entity.h"
#include "ImGui3D.h"
#include "ACU/ThrowTargetPrecision.h"
void OverrideThrowPredictorBeamPosition(AllRegisters* params)
{
    // At this injection point (0x14055B4BB) RBX == ThrowTargetPrecision* and takes two values:
    // two systems that receive messages about the predictor beam's results.
    // One regulates camera rotation around Z, the other one - rotation around camera's left-right axis.
    Entity* player = ACU::GetPlayer();
    static Vector4f farthestResult;
    ThrowTargetPrecision* thr = (ThrowTargetPrecision*)params->rbx_;
    ImGui3D::DrawLocationNamed((Vector3f&)thr->predictionBeamOrigin, "Prediction beam origin");
    ImGui3D::DrawLocationNamed((Vector3f&)thr->trackerCrawlsTowardPredictorBeamEnd, "Prediction beam terminus");
    thr->trackerCrawlsTowardPredictorBeamEnd.z = player->GetPosition().z + 1;
    //(Vector3f&)thr->cameraTrackerMovingTowardPredictionBeamEnd = player->GetPosition() + Vector3f{ 0, 3, 1 };
}
struct PlayWithBombAimCameraTracker2 : AutoAssemblerCodeHolder_Base
{
    PlayWithBombAimCameraTracker2()
    {
        PresetScript_CCodeInTheMiddle(
            0x14055B4BB, 8
            , OverrideThrowPredictorBeamPosition
            , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
            , true);
    }
};

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
struct InputInjection_CycleEquipmentWhenScrollingMousewheel : AutoAssemblerCodeHolder_Base
{
    InputInjection_CycleEquipmentWhenScrollingMousewheel()
    {
        constexpr uintptr_t whenCheckingKeymapForFrame = 0x14273BC75;
        PresetScript_CCodeInTheMiddle(
            whenCheckingKeymapForFrame, 6
            , InjectNumpad46WhenScrollingMouseWheel
            , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
            , true);
    }
};


#include "Hack_ModifyAimingFOV.h"
class MyHacks
{
public:
    AutoAssembleWrapper<EnterWindowWhenRisPressed> enteringWindows;
    AutoAssembleWrapper<AllowSlowMenacingWalkAndAutowalk> menacingWalkAndAutowalk;
    AutoAssembleWrapper<PlayWithFOV> fovGames;
    AutoAssembleWrapper<PlayWithBombAimCameraTracker2> bombAimExperiments2;
    AutoAssembleWrapper<ModifyConditionalFOVs> modifyConditionalFOVs;
    AutoAssembleWrapper<InputInjection_CycleEquipmentWhenScrollingMousewheel> cycleEquipmentOnMouseWheel;

    template<class Hack>
    void DrawCheckboxForHack(Hack& hack, const std::string_view& text)
    {
        if (auto* instance = &hack)
        {
            bool isActive = instance->IsActive();
            if (ImGui::Checkbox(text.data(), &isActive))
            {
                instance->Toggle();
            }
        }
    }
    void ToggleDefaultHacks()
    {
        printf("Toggle.\n");
        if (!enteringWindows.IsActive())
        {
            enteringWindows.Activate();
            menacingWalkAndAutowalk.Activate();
            modifyConditionalFOVs.Activate();
            cycleEquipmentOnMouseWheel.Activate();
        }
        else
        {
            enteringWindows.Deactivate();
            menacingWalkAndAutowalk.Deactivate();
            modifyConditionalFOVs.Deactivate();
            cycleEquipmentOnMouseWheel.Deactivate();
        }
    }
    void OnKeyJustPressed(int keyCode)
    {
        switch (keyCode)
        {
        case VK_NUMPAD6:
            ToggleDefaultHacks();
        default:
            break;
        }
    }
    void DrawControls()
    {
        DrawCheckboxForHack(enteringWindows, "Enter windows by pressing R");
        DrawCheckboxForHack(menacingWalkAndAutowalk, "Allow Autowalk and the Slow Menacing Walk");
        DrawCheckboxForHack(modifyConditionalFOVs, "Modify conditional FOVs");
        DrawCheckboxForHack(cycleEquipmentOnMouseWheel, "Cycle through equipment using mouse wheel");
        DrawCheckboxForHack(fovGames, "Play with FOV");
        DrawCheckboxForHack(bombAimExperiments2, "Bomb aim experiments2");
    }
};
std::optional<MyHacks> g_MyHacks;
void DrawHacksControls()
{
    if (g_MyHacks)
    {
        g_MyHacks->DrawControls();
    }
}

#include "MyVariousHacks.h"


void MyVariousHacks::Start()
{
    g_MyHacks.emplace();
    g_MyHacks->ToggleDefaultHacks();
}
void MyVariousHacks::MyHacks_OnKeyJustPressed(int keyCode)
{
    if (g_MyHacks)
    {
        g_MyHacks->OnKeyJustPressed(keyCode);
    }
}

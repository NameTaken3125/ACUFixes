#include "pch.h"

#include "Hack_LookbehindButton.h"

#include "ACU/InputContainer.h"
#include "ACU_InputUtils.h"
#include "ACU/ACUPlayerCameraComponent.h"

float bring_to_interval_with_wraparound(float current, float min_, float max_)
{
    const float interval = max_ - min_;
    while (current >= max_)
    {
        current -= interval;
    }
    while (current < min_)
    {
        current += interval;
    }
    return current;
}
bool g_IsLookingBehindNow = false;
void WhenReadingCameraRotationForMovement_PretendYoureMovingInOppositeDirection(AllRegisters* params)
{
    if (!g_IsLookingBehindNow)
    {
        return;
    }
    InputContainerBig* inpCont = ACU::Input::Get_InputContainerBig();
    if (!inpCont) { return; }
    Quatf& cameraRotationUsedToCalculateMovementDirection = (Quatf&)params->XMM2.fa;
    cameraRotationUsedToCalculateMovementDirection = Quatf::fromMatrix(cameraRotationUsedToCalculateMovementDirection.rotMatrix() * Matrix3f::createRotationAroundAxis(0, 0, 180));
}
#include "ACU/HumanStatesHolder.h"
bool IsInBombAimMode(ACUPlayerCameraComponent* cameraCpnt);
bool IsInBombAimFromBehindCoverMode(ACUPlayerCameraComponent* cameraCpnt);
bool IsBallisticAimingNow(ACUPlayerCameraComponent& cameraCpnt)
{
    return IsInBombAimMode(&cameraCpnt) || IsInBombAimFromBehindCoverMode(&cameraCpnt);
}
BallisticProjectileAimingProcess& GetLastBallisticAimingProcess(HumanStatesHolder& humanStates)
{
    switch (humanStates.ballisticAimingCurrentEquipmentType)
    {
    case EquipmentType::SmokeBomb:
        return humanStates.aimingSmokeBomb;
        break;
    case EquipmentType::MoneyPouch:
        return humanStates.aimingMoneyPouch;
        break;
    case EquipmentType::CherryBomb:
        return humanStates.aimingCherryBomb;
        break;
    case EquipmentType::PoisonBomb:
        return humanStates.aimingPoison;
        break;
    case EquipmentType::unk_0x19:
        return humanStates.aiming_equip19_1770;
        break;
    case EquipmentType::GuillotineGun:
        return humanStates.aimingGuillotineGun;
        break;
    default:
        return humanStates.aimingDefault;
    }
}
void WhenUpdatingCameraVirtualRotation_FlipWhenMiddleButtonIsPressed(AllRegisters* params)
{
    ACUPlayerCameraComponent& cam = *(ACUPlayerCameraComponent*)params->r14_;
    constexpr float pi = 3.141592653589793238462643383279502884f;
    bool lookbehindStatusChanged = false;
    bool doLookbehind = false;
    if (ACU::Input::IsJustPressed(MouseButton::Middle))
    {
        lookbehindStatusChanged = true;
        doLookbehind = true;
    }
    else if (ACU::Input::IsJustReleased(MouseButton::Middle))
    {
        lookbehindStatusChanged = true;
        doLookbehind = false;
    }
    if (lookbehindStatusChanged)
    {
        g_IsLookingBehindNow = doLookbehind;
        cam.spinaroundAngleZtarget = bring_to_interval_with_wraparound(cam.spinaroundAngleZtarget + pi, -pi, pi);
        cam.spinaroundAngleZcurrent = cam.spinaroundAngleZtarget;
        const bool isBallisticAiming = IsBallisticAimingNow(cam);
        if (!isBallisticAiming)
        {
            cam.disableCameraSmoothingForThisFrame = 1;
        }
        else
        {
            auto* humanStates = HumanStatesHolder::GetForPlayer();
            if (!humanStates)
            {
                return;
            }
            BallisticProjectileAimingProcess& aimingForCurrentBallisticType = GetLastBallisticAimingProcess(*humanStates);
            ((Vector2f&)aimingForCurrentBallisticType.currentAimingState.vecForward_sorta_1b8) *= -1;
        }
    }
}
LookbehindButton::LookbehindButton()
{
    uintptr_t whenReadingCameraDirectionForMovement = 0x141F027A5;
    PresetScript_CCodeInTheMiddle(whenReadingCameraDirectionForMovement, 5,
        WhenReadingCameraRotationForMovement_PretendYoureMovingInOppositeDirection, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenUpdatingCameraVirtualRotation = 0x141F3F88A;
    PresetScript_CCodeInTheMiddle(whenUpdatingCameraVirtualRotation, 8,
        WhenUpdatingCameraVirtualRotation_FlipWhenMiddleButtonIsPressed, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

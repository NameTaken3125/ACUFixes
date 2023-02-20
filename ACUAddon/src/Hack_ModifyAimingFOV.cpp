#include "pch.h"

#include "Hack_ModifyAimingFOV.h"

#include "ACU/CameraSelectorBlenderNode.h"
#include "ACU/CameraData.h"
#include "ACU/CurvePoints.h"

#include "MovabilityUtils.h"

class ObjectRegistry_Entry
{
public:
    CameraSelectorBlenderNode* node;
    uint64 qword_8;
    uint64 hash_mb;
};
assert_sizeof(ObjectRegistry_Entry, 0x18);



class AutoRestoredValue
{
public:
    std::reference_wrapper<float> valueRef;
    float initialValue;
public:
    NON_MOVABLE(AutoRestoredValue)
    AutoRestoredValue(float& valueRef) : valueRef(valueRef), initialValue(valueRef) {}
    ~AutoRestoredValue()
    {
        valueRef.get() = initialValue;
    }
};

#include "ACU/GameStatsManager.h"

namespace ACU
{
// I'm not sure if this really never fails.
Clock& GetClock_UnpausedGame()
{
    return *GameStatsManager::GetSingleton()->GetClock_UnpausedGame();
}
float GetCurrentTime_UnpausedGame()
{
    return GetClock_UnpausedGame().GetCurrentTimeFloat();
}
float GetCurrentDeltaTime_UnpausedGame()
{
    return GetClock_UnpausedGame().deltaTime_mb;
}
} // namespace ACU

class FOVCurveAccessor
{
    std::vector<std::unique_ptr<AutoRestoredValue>> m_AutoRestoredValues;
    void SaveAllCurvePointsForFutureAccessAndRestoration(ObjectRegistry_Entry* cameraMode)
    {
        std::vector<std::unique_ptr<AutoRestoredValue>> savedValuesBackups;
        auto& horizontalPts = cameraMode->node->cameraData->horizontalCurvePts;
        savedValuesBackups.reserve((size_t)horizontalPts.size * 3);
        for (auto& horPt : horizontalPts)
        {
            for (auto& verPt : horPt->verticalCurvePts)
            {
                float& fovRef = verPt->pointData.fov;
                savedValuesBackups.push_back(std::make_unique<AutoRestoredValue>(fovRef));
            }
        }
        m_AutoRestoredValues = std::move(savedValuesBackups);
    }
public:
    FOVCurveAccessor(ObjectRegistry_Entry* cameraMode)
    {
        SaveAllCurvePointsForFutureAccessAndRestoration(cameraMode);
    }
private:
    float m_CurrentEffectiveFOV;
public:
    float GetCurrentEffectiveFOV() { return m_CurrentEffectiveFOV; }
    void SetEffectiveFOV(float value) { SetAllPointsInAllCurvesToConstantValue(value); m_CurrentEffectiveFOV = value; }
private:
    void SetAllPointsInAllCurvesToConstantValue(float value)
    {
        // While aiming bombs, the FOV is actually determined by interpolating a curve
        // specific to this mode. For example, in the case of aiming while standing not-behind-cover,
        // there is a single curve with three curve values something like [0.436, 0.436, 0.785].
        // Presumably the first and last values are at the edges of interpolation, while the middle one lies somewhere between.
        // The "t" value of interpolation is _perhaps_ the rotation of the camera around the left-right axis.
        // For example, if you examine the ACUPlayerCameraComponent::fov while aiming a bomb, you're likely to find
        // values of somewhere around `0.55`. This suggests that the curve is usually evaluated somewhere
        // between the middle and right points. As a result, just by changing the middle value, the
        // curve evaluation will produce a value between `newMiddleValue` and `0.785`.
        // So by setting the middle value to `1.0f`, you'll start seeing actual FOVs of around `0.88`.
        // FOV will therefore be increased but not constant, thus retaining some variation.
        // However, I have found that I like the more consistent feel produced by a flat constant curve
        // (i.e. all points set to the same value).
        for (auto& ref : m_AutoRestoredValues)
        {
            ref->valueRef.get() = value;
        }
    }
};
class FOVCurvesDatabase
{
public:
    std::optional<FOVCurveAccessor> curve_BombAim;
    std::optional<FOVCurveAccessor> curve_BombAimFromBehindCover;

    NON_MOVABLE(FOVCurvesDatabase);
    FOVCurvesDatabase() = default;
    static FOVCurvesDatabase& GetSingleton() { static FOVCurvesDatabase inst; return inst; }
};

constexpr uint64 objHash_BombAimRegular = 0x12F9251F30;
constexpr uint64 objHash_BombAimFromCover = 0x34CE205063;
constexpr float g_newFOVwhileAimingBomb = 1.0f; // = 1.5f;
constexpr float g_newFOVwhileAimingBombFromBehindCover = 1.0f;
/*
When player is aiming a bomb throw, the camera "tries to" follow the predicted landing position
(actually, it seems to gradually follow a tracker that gradually follows the predicted landing position).
The algorith that "follows" the predicted location is kinda jumpy.
For example: if the player tries to aim upwards at a vertical pole (e.g. a thin chimney),
then small left-right changes will cause the predicted landing location to jump on and off the pole,
and so between on-the-pole and on-the-ground-behind-and-possibly-quite-far-below-the-pole.
In addition to this, the camera's FOV decreases while aiming, and these sudden changes
in the camera's tracked location mean big sudden changes in what's visible, resulting in disconnect between input (small mouse movement)
and action (a lot of camera movement). Because the camera doesn't _strictly_ track the landing location,
sudden changes also mean that in some cases the landing location is not even be visible.
Same jankiness happens when aiming through a fence or something with holes in it wide enough
for the predictor beam to pass through sometimes.
It's all pretty annoying.
While I don't have a fix for the tracking algorithm, I have found that simply not-decreasing the FOV
results in aiming experience that is less jumpy and succeeds more often in keeping the landing location
in sight.
So this is the current fix:
- I change the FOV values that are used under certain conditions.
*/
/*
What I'm doing here:
When starting to aim (gradual zoom-in period):
[[[[[[[[[[[14521AAD0]+40]]+9c0]+18]+20]+2f0]]+8]+8]+6c] == float FOV for aim-throw-bomb mode.
     [[[[[[14521AAD0]+40]]+9c0]+18]+20] == [[[ ACUCameraComponent +9c0]+18]+20] == `CameraData*`
`CameraData` describes a "camera action" (for example, the gradual zoom when starting to aim).
This pointer can be found can be found by finding corresponding _unchanging_ `CameraSelectorBlenderNode`.
Specifically for the situation when the player is aiming with a bomb while standing (not behind cover),
the node can be found in the registry of "ObjectRegistry_Entry" with a hash of `12F9251F30` (this hash gives only one result on memory scan).

When aiming bomb from behind cover:
- Hash == 0x34CE205063.
- There are 4 HorizontalCurvePoints, which I think correspond to 4 cardinal directions relative to the cover direction.
*/
void WhenCameraBlendingModeChanged_HijackConditionalFOVs(AllRegisters* params)
{
    ObjectRegistry_Entry* newCameraMode = (ObjectRegistry_Entry*)params->rbx_;
    if (newCameraMode->hash_mb == objHash_BombAimRegular)
    {
        auto& fovCurves = FOVCurvesDatabase::GetSingleton();
        if (!fovCurves.curve_BombAim)
        {
            fovCurves.curve_BombAim = FOVCurveAccessor(newCameraMode);
            fovCurves.curve_BombAim->SetEffectiveFOV(g_newFOVwhileAimingBomb);
        }
    }
    else if (newCameraMode->hash_mb == objHash_BombAimFromCover)
    {
        auto& fovCurves = FOVCurvesDatabase::GetSingleton();
        if (!fovCurves.curve_BombAimFromBehindCover)
        {
            fovCurves.curve_BombAimFromBehindCover = FOVCurveAccessor(newCameraMode);
            fovCurves.curve_BombAimFromBehindCover->SetEffectiveFOV(g_newFOVwhileAimingBombFromBehindCover);
        }
    }
}
template<typename floatlike>
floatlike simple_interp_using_game_time(floatlike mn, floatlike mx)
{
    auto now = ACU::GetCurrentTime_UnpausedGame();
    float speed = 1.5f;
    float interp = sin(now * speed);
    interp = (interp + 1) / 2;
    return mn + (mx - mn) * interp;
}
namespace Interpolations
{

float clamp01(float x) {
    if (x < 0) return 0;
    if (x >= 1) return 1;
    return x;
}
float inverse_smoothstep(float y) {
    y = clamp01(y);
    return (float)(0.5 - sin(asin(1.0 - 2.0 * y) / 3.0));
}
float smoothstep(float x) {
    if (x < 0) return 0;
    if (x >= 1) return 1;
    return x * x * (3 - 2 * x);
}
float smoothstep_arbitrary(float from, float to, float xinRange01) {
    float y = smoothstep(xinRange01);
    return y * to + (1 - y) * from;
}
float smoothstep_reverse_arbitrary(float fromY, float toY, float currentY)
{
    // Scale/bias into [0..1] range
    float yInRange01 = (currentY - fromY) / (toY - fromY);
    float xInRange01 = inverse_smoothstep(yInRange01);
    return xInRange01;
}
} // namespace Interpolations
#include "ACU/ACUPlayerCameraComponent.h"
bool IsInBombAimMode(ACUPlayerCameraComponent* cameraCpnt)
{
    return cameraCpnt->currentCameraSelectorBlenderNode->hash_mb == objHash_BombAimRegular;
}
bool IsInBombAimFromBehindCoverMode(ACUPlayerCameraComponent* cameraCpnt)
{
    return cameraCpnt->currentCameraSelectorBlenderNode->hash_mb == objHash_BombAimFromCover;
}
class FOVWhileAimingManager_AugmentedZoomOnRightClick
{
    const float m_goalFOVWhileAimingWithoutRMB = g_newFOVwhileAimingBomb;
    const float m_goalFOVWhileAimingAndPressingRMB = 0.60f;
public:
    void AugmentZoomDependingOnRMB(FOVCurvesDatabase& fovCurves)
    {
        FOVCurveAccessor& cameraModeController = *fovCurves.curve_BombAim;
        //cameraModeController.SetEffectiveFOV(g_newFOVwhileAimingBomb);
        //return;
        const bool isRMBpressed = GetAsyncKeyState(VK_RBUTTON);
        const float targetFOV = isRMBpressed ? m_goalFOVWhileAimingAndPressingRMB : m_goalFOVWhileAimingWithoutRMB;
        const float currentEffectiveFOV = cameraModeController.GetCurrentEffectiveFOV();
        float newEffectiveFOV = CalculateNewEffectiveFOVForTarget(targetFOV, currentEffectiveFOV);
        cameraModeController.SetEffectiveFOV(newEffectiveFOV);
    }
    void AugmentZoomDependingOnRMB_smoothstep(FOVCurvesDatabase& fovCurves)
    {
        FOVCurveAccessor& cameraModeController = *fovCurves.curve_BombAim;
        const bool isRMBpressed = GetAsyncKeyState(VK_RBUTTON);
        const auto [interpTo, interpFrom] =
            isRMBpressed
            ? std::pair{ m_goalFOVWhileAimingAndPressingRMB, m_goalFOVWhileAimingWithoutRMB }
            : std::pair{ m_goalFOVWhileAimingWithoutRMB, m_goalFOVWhileAimingAndPressingRMB };
        const float currentEffectiveFOV = cameraModeController.GetCurrentEffectiveFOV();

        constexpr float reasonableTimeForInterpolation = 0.3f;
        float currentInterpTee = Interpolations::smoothstep_reverse_arbitrary(interpFrom, interpTo, currentEffectiveFOV);
        float howLongHasPassedSinceStartedInterpolating = currentInterpTee * reasonableTimeForInterpolation;
        const float deltaT = ACU::GetCurrentDeltaTime_UnpausedGame();
        float newInterpTee = (howLongHasPassedSinceStartedInterpolating + deltaT) / reasonableTimeForInterpolation;
        float newEffectiveFOV = Interpolations::smoothstep_arbitrary(interpFrom, interpTo, newInterpTee);

        cameraModeController.SetEffectiveFOV(newEffectiveFOV);
    }
private:
    float CalculateNewEffectiveFOVForTarget(float targetValue, float currentValue)
    {
        constexpr float eps = 0.01f;
        float d = targetValue - currentValue;
        if (abs(d) < eps) { return targetValue; }
        constexpr float reasonableTimeForInterpolation = 0.5f;
        constexpr float reasonablePresumedMaxDistanceBetweenTargetAndCurrentValues = 1.0f;
        constexpr float linearInterpolationSpeed = reasonablePresumedMaxDistanceBetweenTargetAndCurrentValues / reasonableTimeForInterpolation;
        float step = linearInterpolationSpeed * ACU::GetCurrentDeltaTime_UnpausedGame();
        float stepSigned = targetValue > currentValue ? step: -step;
        float newD = targetValue - (currentValue + stepSigned);
        bool dSignChanged = (d > 0 && newD < 0) || (d < 0 && newD > 0);
        if (dSignChanged) { return targetValue; }
        return currentValue + stepSigned;
        // This needs some nice looking interpolation, sigmoid or smth? Preferably stateless.
        return targetValue;
    }
public:
    NON_MOVABLE(FOVWhileAimingManager_AugmentedZoomOnRightClick);
    FOVWhileAimingManager_AugmentedZoomOnRightClick() = default;
    static FOVWhileAimingManager_AugmentedZoomOnRightClick& GetSingleton() { static FOVWhileAimingManager_AugmentedZoomOnRightClick inst; return inst; }
};
void UpdateConditionalFOVCurves(ACUPlayerCameraComponent* cameraCpnt)
{
    auto& fovCurves = FOVCurvesDatabase::GetSingleton();
    if (IsInBombAimMode(cameraCpnt))
    {
        if (fovCurves.curve_BombAim)
        {
            FOVWhileAimingManager_AugmentedZoomOnRightClick::GetSingleton().AugmentZoomDependingOnRMB_smoothstep(fovCurves);
        }
    }
    else if (IsInBombAimFromBehindCoverMode(cameraCpnt))
    {
        if (fovCurves.curve_BombAimFromBehindCover)
        {
            fovCurves.curve_BombAimFromBehindCover->SetEffectiveFOV(g_newFOVwhileAimingBombFromBehindCover);
        }
    }
}
void WhenCameraUpdateStarts(AllRegisters* params)
{
    UpdateConditionalFOVCurves((ACUPlayerCameraComponent*)params->r14_);
}
ModifyConditionalFOVs::ModifyConditionalFOVs()
{
    constexpr uintptr_t onCameraBlendingModeChanged = 0x141F4E4DE;
    PresetScript_CCodeInTheMiddle(
        onCameraBlendingModeChanged, 7
        , WhenCameraBlendingModeChanged_HijackConditionalFOVs
        , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
        , true);


    constexpr uintptr_t onCameraUpdateStarts = 0x141F3F1EE;
    PresetScript_CCodeInTheMiddle(
        onCameraUpdateStarts, 7
        , WhenCameraUpdateStarts
        , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
        , true);
}

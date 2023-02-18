#include "pch.h"

#include "Hack_ModifyAimingFOV.h"

#include "ACU/CameraSelectorBlenderNode.h"
#include "ACU/CameraData.h"
#include "ACU/CurvePoints.h"

struct ObjectRegistry_Entry
{
    CameraSelectorBlenderNode* node;
    uint64 qword_8;
    uint64 hash_mb;
};
assert_sizeof(ObjectRegistry_Entry, 0x18);



class AutoRestoredValue
{
    std::reference_wrapper<float> valueRef;
    float initialValue;
public:
    AutoRestoredValue(float& valueRef) : valueRef(valueRef), initialValue(valueRef) {}
    ~AutoRestoredValue()
    {
        valueRef.get() = initialValue;
    }
};

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
It's all pretty annoying.
While I don't have a fix for the tracking algorithm, I have found that simply not-decreasing the FOV
results in aiming experience that is much less jumpy.
So this is the current fix:
- I change the "target" FOV values that are used under certain conditions.
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
  For each of those I change the `verticalCurvePts[1].fov`.
*/
void WhenCameraBlendingModeChanged_HijackConditionalFOVs(AllRegisters* params)
{
    constexpr uint64 objHash_BombAimRegular = 0x12F9251F30;
    constexpr uint64 objHash_BombAimFromCover = 0x34CE205063;
    // The resulting FOV will actually be somewhat different (as a result of some
    // curve interpolation or blending with other camera modifiers, I imagine).
    constexpr float newFOVwhileAimingBomb = 1.5f;
    ObjectRegistry_Entry* newCameraMode = (ObjectRegistry_Entry*)params->rbx_;
    if (newCameraMode->hash_mb == objHash_BombAimRegular)
    {
        float& fovWhileAimingBomb = newCameraMode->node->cameraData->horizontalCurvePts[0]->verticalCurvePts[1]->pointData.fov;
        static AutoRestoredValue __autoRestoreValue{ fovWhileAimingBomb };
        fovWhileAimingBomb = newFOVwhileAimingBomb;
    }
    else if (newCameraMode->hash_mb == objHash_BombAimFromCover)
    {
        static std::vector<AutoRestoredValue> __autoRestoreValues = [&]() {
            std::vector<AutoRestoredValue> savedValuesBackups;
            auto& horizontalPts = newCameraMode->node->cameraData->horizontalCurvePts;
            savedValuesBackups.reserve(horizontalPts.size);
            for (auto& horPt : horizontalPts)
            {
                float& fovRef = horPt->verticalCurvePts[1]->pointData.fov;
                savedValuesBackups.push_back(fovRef);
                fovRef = newFOVwhileAimingBomb;
            }
            return savedValuesBackups;
        }();
    }
}
ModifyConditionalFOVs::ModifyConditionalFOVs()
{
    constexpr uintptr_t onCameraBlendingModeChanged = 0x141F4E4DE;
    PresetScript_CCodeInTheMiddle(
        onCameraBlendingModeChanged, 7
        , WhenCameraBlendingModeChanged_HijackConditionalFOVs
        , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
        , true);
}

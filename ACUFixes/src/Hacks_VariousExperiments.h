#pragma once

/*
* Currently just junk I was hesitant to delete.
* Only included in the `Hacks_ImGuiControl.cpp`
*/

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"
#include "ImGui3D.h"

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
#include "ACU/HumanStatesHolder.h"
void WhenAimBombBehindCoverSetPredictorBeamOriginOffsetFromCenterMass_Override(AllRegisters* params)
{
    Vector4f& calculatedOffset = *(Vector4f*)(params->GetRSP() + 0x20);
    auto humanStates = (HumanStatesHolder*)params->rbx_;
    (Vector2f&)calculatedOffset = (Vector2f&)humanStates->player->GetDirectionForward();
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

        auto SetBallisticAimingOriginToFrontOfPlayer = [&]()
        {
            uintptr_t whenAimBombBehindCoverSetPredictorBeamOriginOffsetFromCenterMass = 0x1419F97CE;
            PresetScript_CCodeInTheMiddle(whenAimBombBehindCoverSetPredictorBeamOriginOffsetFromCenterMass, 5,
                WhenAimBombBehindCoverSetPredictorBeamOriginOffsetFromCenterMass_Override, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
        };
        //SetBallisticAimingOriginToFrontOfPlayer();
    }
};

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct BetterAimingFromBehindCover : AutoAssemblerCodeHolder_Base
{
    BetterAimingFromBehindCover();
};
BetterAimingFromBehindCover::BetterAimingFromBehindCover()
{
    const uintptr_t whenDecidingIfPlayerIsCloseEnoughToTheEdgeOfCoverToAimBombsInThatDirection = 0x1419623FC;
    PresetScript_NOP(whenDecidingIfPlayerIsCloseEnoughToTheEdgeOfCoverToAimBombsInThatDirection, 2);
}

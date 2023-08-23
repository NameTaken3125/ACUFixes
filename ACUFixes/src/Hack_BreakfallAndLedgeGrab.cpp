#include "pch.h"

#include "Hack_BreakfallAndLedgeGrab.h"

#include "ImGui3D.h"
#include "Common_Plugins/ACU_InputUtils.h"
Vector4f g_SavedBreakfallScannerPosition;
void WhenInFreefall_RestoreModifiedVectors(AllRegisters* params)
{
    Vector4f* playerFeetPosition = (Vector4f*)params->rbp_;
    *playerFeetPosition = g_SavedBreakfallScannerPosition;
}
void WhenInFreefall_HelpFindLedgeToGrab(AllRegisters* params)
{
    Vector3f* playerFeetPosition = (Vector3f*)params->rbp_;
    // If I don't restore the modified position vector, it gets used for the "bump agains the wall" test.
    // This means that if Arno is falling close enough to the wall, but there just isn't anything to grab onto,
    // then he will keep repeatedly bumping into the wall on his way down.
    g_SavedBreakfallScannerPosition = *(Vector4f*)playerFeetPosition;
    if (!ACU::Input::IsPressed(ActionKeyCode::ParkourDownInteract))
        return;
    //Vector3f* vecTowardWall = (Vector3f*)params->rsi_;
    Vector3f* playerMovementVecInGroundPlane = (Vector3f*)params->r14_;
    Vector3f& searchDirectionInGroundPlane_UnitVector = *playerMovementVecInGroundPlane;
    constexpr float pretendPlayerIsCloserToWall_multiplier = 1.0f;
    *playerFeetPosition = *playerFeetPosition + searchDirectionInGroundPlane_UnitVector * pretendPlayerIsCloserToWall_multiplier;
    ImGui3D::DrawLocationNamed(*playerFeetPosition, "Breakfall: sensor location");
}
BreakfallAndLedgeGrab::BreakfallAndLedgeGrab()
{
    uintptr_t whenInFreefallTestForBreakfall = 0x141A3C0E1;
    PresetScript_CCodeInTheMiddle(whenInFreefallTestForBreakfall, 5,
        WhenInFreefall_HelpFindLedgeToGrab, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenInFreefallTestForBreakfall_functionEpilogue = 0x141A3C293;
    PresetScript_CCodeInTheMiddle(whenInFreefallTestForBreakfall_functionEpilogue, 5,
        WhenInFreefall_RestoreModifiedVectors, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

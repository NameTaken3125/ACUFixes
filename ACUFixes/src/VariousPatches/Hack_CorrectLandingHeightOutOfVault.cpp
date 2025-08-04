#include "pch.h"

#include "Hack_CorrectLandingHeightOutOfVault.h"

#include "ACU/AtomGraph.h"
#include "ACU/AtomGraphEvaluation.h"
#include "ACU_DefineNativeFunction.h"
#include "ParkourDebugging/AvailableParkourAction.h"


DEFINE_GAME_FUNCTION(AtomGraph_RTCP__SetRTCP4Bytes, 0x1400A3A30, void, __fastcall, (AtomGraph_RTCP* a1, unsigned int p_rtcpVarIdx, GraphEvaluation_400* a3, float* p_value));
void WhenInitiatingVault_SetRTCP_TargetHeight_theSameAs_LandingHeight(AllRegisters* params)
{
    /*
    Parkour down from the VShape of the tree at [-407.33 -133.18 0.04] is broken _sometimes_.
    I cannot establish if this patch is related somehow.
    Perhaps I can make a distinction by EnumParkourAction?
    Tree VShape:
        15 == jumpDownToGround_airtimeButSafe
    Vaults at [-327.65 -151.04 0.00]:
        9 == riseFromGroundToGround_smallDiff_alsoVault9

    [-510.43 -38.80 -0.28]:
    Chopping block, fireplace still produce heavy landing.
    UPD: TargetDistance has an influence too.
    */

    // The TargetDistance RTCP also influences the landing animation.
    GraphEvaluation& graphEval = *(GraphEvaluation*)params->rbx_;
    // scalar TargetHeight; // 0xdc434c51/3695397969
    // scalar TargetDistance; // 0x97a0d819/2543900697
    constexpr uint32 rtcpHash_TargetHeight = 0xdc434c51;
    constexpr uint32 rtcpHash_TargetDistance = 0x97a0d819;
    float rtcpValue_LandingHeight = *(float*)(params->GetRSP() + 0xE0);
    auto SetRTCPFloat = [&](uint32 hash, float value) {
        uint32* rtcpIdx = graphEval.rtcp->atomGraphVarsHashmap.Get(hash);
        if (!rtcpIdx) return;
        AtomGraph_RTCP__SetRTCP4Bytes(graphEval.rtcp, *rtcpIdx, &graphEval.stru400, &value);
        };
    SetRTCPFloat(rtcpHash_TargetHeight, rtcpValue_LandingHeight);
    //SetRTCPFloat(rtcpHash_TargetDistance, landingHeightValue * -1.6350f);
    AvailableParkourAction& action = *(AvailableParkourAction*)params->rbp_;
    // action.horizontalDifference is also used for
    //      scalar SyncA_TargetDistance; // 0x114ad6d6/290117334
    // I don't know the meaning of these Sync... variables (D and E aren't used here,
    // and A, B, C all differ), but maybe this works well enough.
    SetRTCPFloat(rtcpHash_TargetDistance, action.horizontalDifference);
}

CorrectLandingHeightOutOfVault::CorrectLandingHeightOutOfVault()
{
    PresetScript_CCodeInTheMiddle(0x140195FF4, 8,
        WhenInitiatingVault_SetRTCP_TargetHeight_theSameAs_LandingHeight, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

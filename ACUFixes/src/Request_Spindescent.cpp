#include "pch.h"

#include "Request_Spindescent.h"

#include "vmath/vmath.h"

#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"


enum class EnumParkourAction
{
    unk0 = 0x0,
    stumbleAtEdge_mb = 0x1,
    passement2 = 0x2,
    leap_3_slowStepDown_mb = 0x3,
    leapBeamToVShape = 0x4,
    unk5 = 0x5,
    unk6 = 0x6,
    unk7 = 0x7,
    unk8 = 0x8,
    passement9 = 0x9,
    passementFromBeam_A = 0xA,
    passementB = 0xB,
    passementFromBeam_C = 0xC,
    passementD = 0xD,
    passement_E = 0xE,
    leap_F = 0xF,
    unk16 = 0x10,
    unk17 = 0x11,
    breakfall_1st_0x12 = 0x12,
    unk19 = 0x13,
    freefallLedgeLandOn = 0x14,
    breakfall_2nd_0x15 = 0x15,
    unk22 = 0x16,
    hangShimmy_17 = 0x17,
    unk24 = 0x18,
    unk25 = 0x19,
    hangCornerInside_1A = 0x1A,
    hangCornerOutside_1B = 0x1B,
    hangCornerInside_1C = 0x1C,
    hangCornerOutside_1D = 0x1D,
    unk30 = 0x1E,
    hang_1F = 0x1F,
    hang_20 = 0x20,
    hangShimmy_21 = 0x21,
    hangTurn_22 = 0x22,
    unk35 = 0x23,
    offTheWall_fromWallToGroundReleaseOrEject = 0x24,
    unk37 = 0x25,
    unk38 = 0x26,
    climbFacade_fromWallDescentSidewaysToGround_alsoSidehop = 0x27,
    dive = 0x28,
    leapOffBuildingIntoUncontrolledFall_mb = 0x29,
    unk42 = 0x2A,
    swing_2B = 0x2B,
    swing_2C = 0x2C,
    swingTurn_2D = 0x2D,
    swing_2E = 0x2E,
    swing_2F_SwingToSidewall_includingSpindescent = 0x2F,
    swing_30_fromSwingToFeet = 0x30,
    swing_31_swingToSwing_mb = 0x31,
    offTheWall_fromWallToWallEjectAndCatch = 0x32, // If you force this you can cover absolutely ridiculous wall-to-wall distances like from [194.55 -335.12 1.71] to [186.31 -331.30 3.76]
    unk51 = 0x33,
    unk52 = 0x34,
    unk53 = 0x35,
    unk54 = 0x36,
    unk55 = 0x37,
    unk56 = 0x38,
    unk57 = 0x39,
    unk58 = 0x3A,
    edgeDropBeforeFreeeFall_3B = 0x3B,
    unk60 = 0x3C,
    unk61 = 0x3D,
    unk62 = 0x3E,
    edgeDropBeforeFreeeFall_3F = 0x3F,
    unk64 = 0x40,
    climb_onWallNormalClimb = 0x41,
    unk66 = 0x42,
    climbCornerInside_onWallTurnCornerInner = 0x43,
    onWallTurnCornerOuter = 0x44,
    unk69 = 0x45,
    fromWallCalmDescentToFeet = 0x46,
    climbCornerInside_47 = 0x47,
    climbCornerOutside = 0x48,
    climb_fromWallRiseToFeetOnLedge_49 = 0x49,
    climb_fromWallRiseToFeetOnLedge_4A = 0x4A,
    climb_fromWallRiseToFeetOnLedge_4B = 0x4B,
    unk76 = 0x4C,
    unk77 = 0x4D,
    hangShimmy_4E = 0x4E,
    backEject_4F = 0x4F,
    climb_50 = 0x50,
    unk81 = 0x51,
    unk82 = 0x52,
    unk83 = 0x53,
    unk84 = 0x54,
    unk85 = 0x55,
    choke_mb = 0x56,
    unk87 = 0x57,
    unk88 = 0x58,
    unk89 = 0x59,
    unk90 = 0x5A,
    unk91 = 0x5B,
    unk92 = 0x5C,
    unk93 = 0x5D,
    unk94 = 0x5E,
    unk95 = 0x5F,
    unk96 = 0x60,
    unk97 = 0x61,
    unk98 = 0x62,
    unk99 = 0x63,
    unk100 = 0x64,
    unk101 = 0x65,
    unk102 = 0x66,
    unk103 = 0x67,
    unk104 = 0x68,
    unk105 = 0x69,
    passement6A_walkThroughWindow_mb = 0x6A,
    passement6B = 0x6B,
    enterWindow = 0x6C,
    unk109 = 0x6D,
    unk110 = 0x6E,
    unk111 = 0x6F,
    unk112 = 0x70,
    unk113 = 0x71,
    passementFromHayStack = 0x72,
    passement_73 = 0x73,
    unk116 = 0x74,
    unk117 = 0x75,
    unk118 = 0x76,
    defenestrateSprintOutOfWindow_77 = 0x77,
    unk120 = 0x78,
    defenestrateSprintOutOfWindow_79 = 0x79,
    defenestrateSprintOutOfWindow_7A = 0x7A,
    unk123 = 0x7B,
    unk124 = 0x7C,
};

#include "ACU/FancyVTable.h"
struct ParkourAction_FancyVTable
{
    std::array<FancyVFunction, 0x78> fancyVirtuals;
};
class AvailableParkourAction
{
public:
    // @members
    uint64 vtbl;
    ParkourAction_FancyVTable* fancyVTable;

    // @helper_functions
    EnumParkourAction GetEnumParkourAction()
    {
        using GetEnumParkourAction_t = EnumParkourAction(__fastcall*)(AvailableParkourAction* action);
        auto GetEnumParkourAction_impl = (GetEnumParkourAction_t)fancyVTable->fancyVirtuals[69].fn;
        return GetEnumParkourAction_impl(this);
    }
};
#include "ACU/SharedPtr.h"
class Entity;
class EntityGroup;
class ParkourAction_Spindescent : public AvailableParkourAction
{
public:
    Vector4f handsLocationFrom; //0x0010
    char pad_0020[16]; //0x0020
    Vector4f handsLocationTo_center_mb; //0x0030
    char pad_0040[16]; //0x0040
    Vector4f handsLocationTo_right_mb; //0x0050
    Vector4f handsLocationTo_left_mb; //0x0060
    Vector4f dir70; //0x0070
    Vector4f dir80; //0x0080
    Vector4f directionFromTo; //0x0090
    Vector4f dirA0; //0x00A0
    char pad_00B0[16]; //0x00B0
    float heightDifference; //0x00C0
    float horizontalDifference; //0x00C4
    float distance; //0x00C8
    char pad_00CC[16]; //0x00CC
    uint32 dword_DC; //0x00DC
    char pad_00E0[8]; //0x00E0
    SharedPtrNew<EntityGroup>* shared_buildingEntityGroup_mb; //0x00E8
    char pad_00F0[16]; //0x00F0
    SharedPtrNew<Entity>* shared_player; //0x0100
    char pad_0108[112]; //0x0108
    SharedPtrNew<EntityGroup>* shared_entityGroupWithVisualCpntsOnly_178_canBeEmpty; //0x0178
    Vector4f vec180; //0x0180
    Vector4f vec190; //0x0190
    Vector4f vec1A0; //0x01A0
    char pad_01B0[256]; //0x01B0
}; //Size: 0x02B0
assert_sizeof(ParkourAction_Spindescent, 0x2B0);
#include "ACU_InputUtils.h"
bool IsShouldTryForceTheSpindescentAnimation()
{
    return ACU::Input::IsPressed(ActionKeyCode::ParkourDownInteract);
}
constexpr int PARKOUR_ACTIONS_NO_RESULTS_ACCEPTED = -1;
std::optional<int> SelectBestMatchingMoveIdx_ExtraProcessing_CustomSelection(AllRegisters* params)
{
    if (!IsShouldTryForceTheSpindescentAnimation())
    {
        return {};
    }
    SmallArray<AvailableParkourAction*>& availableParkourActions = **(SmallArray<AvailableParkourAction*>**)(params->GetRSP() + 8 * 8);
    if (!availableParkourActions.size)
    {
        return PARKOUR_ACTIONS_NO_RESULTS_ACCEPTED;
    }
    for (int i = 0; i < availableParkourActions.size; i++)
    {
        AvailableParkourAction* action = availableParkourActions[i];
        //constexpr uint64 SwingToSidewall_includingSpindescent_StaticNodes = 0x143A3E170;
        //if ((uint64&)action->fancyVTable ==
        //    SwingToSidewall_includingSpindescent_StaticNodes
        //    //0x143A47FA0         // dive, mb?
        //    //0x143A4E1D0         // swing turn
        //    //0x143A13130         // descent from wall to ground; also sidehop
        //    //0x1439FA6C0
        //)
        EnumParkourAction actionType = action->GetEnumParkourAction();
        if (actionType == EnumParkourAction::swing_2F_SwingToSidewall_includingSpindescent)
        {
            /*
            At [608.67 -35.59 13.42], lower from swing, immediately hold Sprint+E+forward.
            If I increase heights for the target locations,
            I can consistently do the spinmove upwards, kinda.
            */
            ParkourAction_Spindescent* spindescent = (ParkourAction_Spindescent*)action;
            auto IncreaseTargetHeight = [&]()
            {
                // This can increase the _actual_ target location, so the spinning descent animation
                // can be played while going upwards, if you want to do that for some reason.
                Vector4f delta = { 0, 0, 4, 0 };
                spindescent->handsLocationTo_center_mb += delta;
                spindescent->handsLocationTo_right_mb += delta;
                spindescent->handsLocationTo_left_mb += delta;
                spindescent->vec180 += delta;
                spindescent->vec190 += delta;
                spindescent->vec1A0 += delta;
            };
            auto ForceSpindescentAnimation = [&]()
            {
                /*
                At [608.49 -35.76 11.22], when transitioning from swing to sidewall using the Mid mode,
                was able to successfully force the spindescent animation
                by substituting 8 floats (using just the first one did the trick too, most of the time)
                at 0xC0.
                Example:
                Midmode:         0.22 3.63 3.64 0.98 0.04 0.24 0.11 0.00
                Spindescent:    -4.04 2.29 4.64 0.92 0.10 0.48 0.05 0.00

                At [562.95 40.07 6.31], this wasn't enough to force the spin.
                Also, this current solution of forcing the swing-to-sidewall
                produces a bug at that spot, where Arno is grabbing the wall
                at the point where it connects with a wooden "balcony".
                */
                spindescent->heightDifference = -4.04f;
                constexpr std::array<float, 8> floatsForSpindescent = { -4.04f, 2.29f, 4.64f, 0.92f, 0.10f, 0.48f, 0.05f, 0.00f };
                (std::array<float, 8>&)spindescent->heightDifference = floatsForSpindescent;
            };
            auto IsTargetSuitedForSpindescent = [&]() -> bool
            {
                return spindescent->heightDifference <= -2.0f;
            };
            if (IsTargetSuitedForSpindescent())
            {
                ForceSpindescentAnimation();
                return i;
            }
        }
    }
    return {};
}
using BestMatchSelector_fnt = int(__fastcall*)(__int64 a1, __int64 a2, __int64 a3, __int64 a4, float a5, int a6, __int64 a7, __int64 a8, SmallArray<AvailableParkourAction*>* p_parkourActions);
int SelectBestMatchingMoveIdx_ExtraProcessing(AllRegisters* params)
{
    std::optional<int> customSelectedAction = SelectBestMatchingMoveIdx_ExtraProcessing_CustomSelection(params);
    if (customSelectedAction)
    {
        return *customSelectedAction;
    }
    SmallArray<AvailableParkourAction*>& availableParkourActions = **(SmallArray<AvailableParkourAction*>**)(params->GetRSP() + 8 * 8);
    int bestMatchIdx = ((BestMatchSelector_fnt)params->r10_)(
        params->rcx_,
        params->rdx_,
        params->r8_,
        params->r9_,
        *(float*)(params->GetRSP() + 8 * 4),
        *(int*)(params->GetRSP() + 8 * 5),
        *(__int64*)(params->GetRSP() + 8 * 6),
        *(__int64*)(params->GetRSP() + 8 * 7),
        &availableParkourActions
        );
    return bestMatchIdx;
}
void WhenDecidingBestMatchingMove_ExtraProcessing(AllRegisters* params)
{
    int bestMatchIdx = SelectBestMatchingMoveIdx_ExtraProcessing(params);
    params->GetRAX() = bestMatchIdx;
    *(int*)(params->GetRSP() + 0x100) = bestMatchIdx;
}
ParkourActionsExtraProcessing::ParkourActionsExtraProcessing()
{
    uintptr_t whenDecidingBestMatchingMove = 0x140134984;
    PresetScript_CCodeInTheMiddle(whenDecidingBestMatchingMove, 10,
        WhenDecidingBestMatchingMove_ExtraProcessing, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
}

#include "pch.h"

#include "Request_Spindescent.h"

#include "vmath/vmath.h"

#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"


class SomeStaticNode
{
public:
    uint32 hash; //0x0000
    uint32 staticIdx; //0x0004
    uint32 nestedness; //0x0008
    char pad_000C[4]; //0x000C
    uint64 fn; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020
assert_sizeof(SomeStaticNode, 0x20);

class AvailableParkourAction
{
public:
    uint64 vtbl;
    uint64 staticNodes;
};
#include "ACU/SharedPtr.h"
class Entity;
class EntityGroup;
class ParkourAction_Spindescent
{
public:
    char pad_0000[16]; //0x0000
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
        constexpr uint64 SwingToSidewall_includingSpindescent_StaticNodes = 0x143A3E170;
        if (action->staticNodes ==
            SwingToSidewall_includingSpindescent_StaticNodes
            //0x143A47FA0         // dive, mb?
            //0x143A4E1D0         // swing turn
            //0x143A13130         // descent from wall to ground; also sidehop
            //0x1439FA6C0
            )
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
            //return PARKOUR_ACTIONS_NO_RESULTS_ACCEPTED;
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

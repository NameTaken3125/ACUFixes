#include "pch.h"

#include "Request_Spindescent.h"

#include "vmath/vmath.h"

#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"

#include "ParkourDebugging/EnumParkourAction.h"

#include "ACU/FancyVTable.h"
struct ParkourAction_FancyVTable
{
    std::array<FancyVFunction, 0x78> fancyVirtuals;
};
// FancyVFunctions in `ParkourAction_EnterWindow.fancyVtable8`:
#define DEFINE_FANCY_VF(idx, name, nameHashed, fnType)

DEFINE_FANCY_VF(0x18, "GetDistance", 0x8EB3C7D3, float (*)(AvailableParkourAction*));
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
    char pad_01B0[84]; //0x01B0
    float simpleFitness; //0x0204
    char pad_0208[168]; //0x0208
}; //Size: 0x02B0
assert_sizeof(ParkourAction_Spindescent, 0x2B0);
#include "Common_Plugins/ACU_InputUtils.h"
#include "MainConfig.h"
bool IsShouldTryForceTheSpindescentAnimation()
{
    return g_Config.personalRequests->parkourHelper->spinningDescentHelper
        && ACU::Input::IsPressed(ActionKeyCode::ParkourDownInteract);
}
static bool IsShouldTryForceDive()
{
    return g_Config.personalRequests->parkourHelper->diveHelper->isActive
        && ACU::Input::IsPressed(g_Config.personalRequests->parkourHelper->diveHelper->hotkey.get());
}
constexpr int PARKOUR_ACTIONS_NO_RESULTS_ACCEPTED = -1;
struct CustomSelectedParkourMove
{
    AvailableParkourAction* m_action;
    int m_idx;
};
CustomSelectedParkourMove SelectBestMatchingMoveIdx_ExtraProcessing_CustomSelection_DiveMove(SmallArray<AvailableParkourAction*>& availableParkourActions)
{
    if (!IsShouldTryForceDive())
    {
        return { nullptr, PARKOUR_ACTIONS_NO_RESULTS_ACCEPTED };
    }
    for (int i = 0; i < availableParkourActions.size; i++)
    {
        AvailableParkourAction* action = availableParkourActions[i];
        EnumParkourAction actionType = action->GetEnumParkourAction();
        if (actionType == EnumParkourAction::dive
            || actionType == EnumParkourAction::swing_31_swingToSwing_mb
            || actionType == EnumParkourAction::jumpOutOfWindowToHang
            )
        {
            return { action, i };
        }
    }
    return { nullptr, PARKOUR_ACTIONS_NO_RESULTS_ACCEPTED };
}
CustomSelectedParkourMove SelectBestMatchingMoveIdx_ExtraProcessing_CustomSelection_Spindescent(SmallArray<AvailableParkourAction*>& availableParkourActions)
{
    if (!IsShouldTryForceTheSpindescentAnimation())
    {
        return { nullptr, PARKOUR_ACTIONS_NO_RESULTS_ACCEPTED };
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

                // See at 140133B74:
                // If the value of `flt_204` is too close to 0, this action
                // will get removed at the start of the sorting stage.
                if (fabs(spindescent->simpleFitness) < 0.01f)
                    spindescent->simpleFitness = 0.01f;
            };
            auto IsTargetSuitedForSpindescent = [&]() -> bool
            {
                return spindescent->heightDifference <= -2.0f;
            };
            if (IsTargetSuitedForSpindescent())
            {
                ForceSpindescentAnimation();
                return { spindescent, i };
            }
        }
    }
    return { nullptr, PARKOUR_ACTIONS_NO_RESULTS_ACCEPTED };
}
using BestMatchSelector_fnt = int(__fastcall*)(__int64 a1, __int64 a2, __int64 a3, __int64 a4, float a5, int a6, __int64 a7, __int64 a8, SmallArray<AvailableParkourAction*>* p_parkourActions);
int SelectBestMatchingMoveIdx_ExtraProcessing(AllRegisters* params)
{
    SmallArray<AvailableParkourAction*>& availableParkourActions = **(SmallArray<AvailableParkourAction*>**)(params->GetRSP() + 8 * 8);
    /*
    The custom move selection is a little weird.
    The idea is simple: the dive (via hotkey Mouse5) takes the highest priority (if available).
    The spindescent takes the highest priority (if available) if the dive wasn't selected.
    If neither was forced, let the game choose the next move normally.

    Here's what I believe about the game's parkour system:
    When the current parkour system (e.g. the `WhileOnWallSystem`) is updated
    for the frame, the following things happen:
    1. Current player input is gathered: camera,
       movement direction (it's a Vector2f, for example "W" key corresponds to vector [0.0, 1.0],
       and "A+W" == [-0.707, 0.707]),
       the current "suggested parkour modifier" (e.g. ParkourUp, ParkourDown, ParkourStraight, TapJump).
    2. Multiple "action scanners" (depending on the current parkour system) such as "Window Entry",
       "Swing Turn", "Freefall Grab", "Turn Corner Inner/Outer" are initialized
       then do their scanning (a scanner object searches for matches to one or many of the `EnumParkourAction`).
    3. Depending on the player's input, scanners find potential parkour locations.
       For example, when swinging on a horizontal bar, holding ParkourUp will only scan for locations above or ahead
       of the player, but holding ParkourDown can scan way below.
    4. The current scanner tests each of the potential locations it found to confirm it indeed fits a move type
       that they scan for (this step is unclear to me).
    5. A subclass of `AvailableParkourAction` corresponding to `EnumParkourAction` is created
       and added to the `availableParkourActions` array.
    6. By running each of the scanners, the `availableParkourActions` array is filled out.
    7. The array of available actions is then sorted according to the value of:
        actionFitness = defaultActionWeightForTheCurrentParkourMode * someCalculatedValueSpecificToTheCurrentEvaluatedAction
       (this step is also not very clear to me)
    8. The newly sorted array of available actions is then walked front to back; a couple of final checks are run
       to determine that all conditions for the move are indeed satisfied
       (for example, the EnterWindowFromWall action can be judged to be not satisfied unless the player
       is fully leaning in the direction of that window).
    9. The first action that is found to be fitting is selected as the next performed move.

    This hook replaces (and optionally performs) steps 7-9 inclusively.
    And here lies the kookiness of my custom selection algorithm:
    I want the dive move to have higher priority than the spindescent, but
    I have the best results when selecting the Spindescent move _before_ the available actions are sorted,
    and the best results for the dive move _after_ the sorting.
    So I search for the spindescent, run the sorting, search for the dive move,
    then make my final selection.
    */
    auto [selectedSpindescent, _spindescentIdxBeforeSorting] = SelectBestMatchingMoveIdx_ExtraProcessing_CustomSelection_Spindescent(availableParkourActions);
    int bestMatchIdxAfterNativeSorting = ((BestMatchSelector_fnt)params->r10_)(
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
    if (g_Config.personalRequests->parkourHelper->canRiseOnLedgeAfterLedgeAssassination)
    {
        if (bestMatchIdxAfterNativeSorting == -1 && availableParkourActions.size > 0)
        {
            const bool allTheActionsAreTryingToRiseOnLedge = std::all_of(availableParkourActions.begin(), availableParkourActions.end(), [](AvailableParkourAction* action)
                {
                    return (uint64)action->fancyVTable == 0x1439F41C0;
                });
            if (allTheActionsAreTryingToRiseOnLedge)
            {
                bestMatchIdxAfterNativeSorting = 0;
            }
        }
    }
    auto [selectedDiveMove, selectedDiveMoveIdx] = SelectBestMatchingMoveIdx_ExtraProcessing_CustomSelection_DiveMove(availableParkourActions);

    if (selectedDiveMove)
    {
        // The dive takes the highest priority.
        return selectedDiveMoveIdx;
    }
    if (selectedSpindescent)
    {
        // The spindescent takes the second highest priority.
        for (int i = 0; i < availableParkourActions.size; i++)
        {
            if (availableParkourActions[i] == selectedSpindescent)
            {
                return i;
            }
        }
    }
    // If nothing was forced, use the move that the game selected.
    return bestMatchIdxAfterNativeSorting;
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

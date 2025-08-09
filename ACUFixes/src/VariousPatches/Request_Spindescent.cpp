#include "pch.h"

#include "Request_Spindescent.h"

#include "ACU/Memory/ACUAllocs.h"
#include "ACU_DefineNativeFunction.h"

#include "ParkourDebugging/AvailableParkourAction.h"


class Entity;
class EntityGroup;
class ParkourAction_Spindescent : public AvailableParkourAction
{
public:
    char pad_0290[0x2B0-0x290]; //0x0208
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
static bool IsShouldTryForceWallEjectToHang()
{
    // Fun fact: to get an almost functional sidehop-to-hang, just NOP 4 bytes at 0x14015423F.
    return
        g_Config.personalRequests->parkourHelper->diveHelper->isActive
        && g_Config.personalRequests->parkourHelper->diveHelper->allowWallEjectToHang
        && ACU::Input::IsPressed(g_Config.personalRequests->parkourHelper->diveHelper->hotkey.get());
}
constexpr int RESULT_OF_PARKOUR_SORT_AND_SELECT__NO_ACTIONS_ACCEPTED = -1;
struct CustomSelectedParkourMove
{
    AvailableParkourAction* m_action;
    int m_idx;
};
CustomSelectedParkourMove SelectBestMatchingMoveIdx_ExtraProcessing_CustomSelection_DiveMove(SmallArray<AvailableParkourAction*>& availableParkourActions)
{
    if (!IsShouldTryForceDive())
    {
        return { nullptr, RESULT_OF_PARKOUR_SORT_AND_SELECT__NO_ACTIONS_ACCEPTED };
    }
    for (int i = 0; i < availableParkourActions.size; i++)
    {
        AvailableParkourAction* action = availableParkourActions[i];
        EnumParkourAction actionType = action->GetEnumParkourAction();
        if (actionType == EnumParkourAction::dive
            || actionType == EnumParkourAction::swing_31_swingToSwing_mb
            || actionType == EnumParkourAction::jumpOutOfWindowToHang
            || actionType == EnumParkourAction::wallEjectToHang
            )
        {
            return { action, i };
        }
    }
    return { nullptr, RESULT_OF_PARKOUR_SORT_AND_SELECT__NO_ACTIONS_ACCEPTED };
}
CustomSelectedParkourMove SelectBestMatchingMoveIdx_ExtraProcessing_CustomSelection_Spindescent(SmallArray<AvailableParkourAction*>& availableParkourActions)
{
    if (!IsShouldTryForceTheSpindescentAnimation())
    {
        return { nullptr, RESULT_OF_PARKOUR_SORT_AND_SELECT__NO_ACTIONS_ACCEPTED };
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
                spindescent->locationAnchorDest += delta;
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
                if (fabs(spindescent->fitness) < 0.01f)
                    spindescent->fitness = 0.01f;
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
    return { nullptr, RESULT_OF_PARKOUR_SORT_AND_SELECT__NO_ACTIONS_ACCEPTED };
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
    //// If I can provide some flexible way to specify custom move selection from UI
    //// without recompilation, then this is where that selection can be enforced.
    //std::optional<int>                 ParkourDebugging_SelectMove(SmallArray<AvailableParkourAction*>&availableParkourActions);
    //std::optional<int> forcedMoveIdx = ParkourDebugging_SelectMove(availableParkourActions);
    //if (forcedMoveIdx) return *forcedMoveIdx;
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
class ParkourAction_Commonbase;
DEFINE_GAME_FUNCTION(CreateParkourActionForParkourPointIfFits, 0x1401D1CF0, char, __fastcall, (EnumParkourAction a1, uint64 a2, __m128* a3, __m128* p_movementVecWorld_mb, float a5, int a6, char a7, uint64 a8, uint64 a9, uint64 p_currentLedge_mb, ParkourAction_Commonbase** p_newAction_out, float a12, float p_epsilon_mb));
void WhenPerformingScanFor_ClimbFacade_AlsoScanForWallToHang(AllRegisters* params)
{
    if (!IsShouldTryForceWallEjectToHang()) return;
    bool isCreated = CreateParkourActionForParkourPointIfFits(
        EnumParkourAction::wallEjectToHang
        , params->rdx_
        , (__m128*)params->rdi_
        , (__m128*)params->r9_
        , params->XMM6.f0
        , (int&)params->r13_
        , *(char*)(params->GetRSP() + 0x100)
        , *(uint64*)(*(uint64*)params->r12_ + (int&)params->r15_)
        , *(uint64*)(params->rbp_ + 0x118)
        , *(uint64*)(params->rbp_ + 0x120)
        , (ParkourAction_Commonbase**)(params->GetRSP() + 0x108)
        , *(float*)(params->rbp_ + 0x104)
        , *(float*)(params->rbp_ + 0x108)
    );
    if (isCreated)
    {
        ACU::Memory::SmallArrayAppend(*(SmallArray<ParkourAction_Commonbase*>*)params->rbx_, *(ParkourAction_Commonbase**)(params->GetRSP() + 0x108));
    }
}
void WhenPerformingScanFor_BackEject_AlsoScanForWallToHang(AllRegisters* params)
{
    if (!IsShouldTryForceWallEjectToHang()) return;
    bool isCreated = CreateParkourActionForParkourPointIfFits(
        EnumParkourAction::wallEjectToHang
        , *(uint64*)(params->GetRSP() + 0xE8)
        , (__m128*)params->rsi_
        , (__m128*)(params->rdi_ + 0x20)
        , params->XMM6.f0
        , (int&)params->r13_
        , *(char*)(params->GetRSP() + 0x110)
        , *(uint64*)(*(uint64*)params->r14_ + (int&)params->r12_)
        , *(uint64*)(params->rdi_ + 0x118)
        , *(uint64*)(params->rdi_ + 0x120)
        , (ParkourAction_Commonbase**)(params->GetRSP() + 0x70)
        , *(float*)(params->rdi_ + 0x104)
        , *(float*)(params->rdi_ + 0x108)
    );
    if (isCreated)
    {
        ACU::Memory::SmallArrayAppend(*(SmallArray<ParkourAction_Commonbase*>*)params->rbx_, *(ParkourAction_Commonbase**)(params->GetRSP() + 0x70));
    }
}
#include "ACU/Parkour_WhileOnWallSystem.h"
constexpr uint64 climbFacadeTester_FancyVTable_default = 0x1439E08D0;
constexpr uint64 climbFacadeTester_FancyVTable_baseclassThatHasSidehopToSwing = 0x1439FA1A0;
void WhenStartingWallingCycle_ChangeFVTables(AllRegisters* params)
{
    WhileOnWallSystem* hsWalling = (WhileOnWallSystem*)params->rbx_;
    void* climbFacadeTester = hsWalling->climbFacade;
    uint64& climbFacadeFancyVTable = *(uint64*)((uint64)climbFacadeTester + 8);
    const bool useSidehopToSwing = IsShouldTryForceWallEjectToHang();
    if (useSidehopToSwing)
    {
        climbFacadeFancyVTable = climbFacadeTester_FancyVTable_baseclassThatHasSidehopToSwing;
    }
    else
    {
        climbFacadeFancyVTable = climbFacadeTester_FancyVTable_default;
    }
}
void WhenStartingFailedWallrunCycle_ChangeFVTables(AllRegisters* params)
{
    uint64 hsFailedWallrun = params->rsi_;
    uint64 climbFacadeTester = *(uint64*)(hsFailedWallrun + 0x1D0);
    uint64& climbFacadeFancyVTable = *(uint64*)(climbFacadeTester + 8);
    const bool useSidehopToSwing = IsShouldTryForceWallEjectToHang();
    if (useSidehopToSwing)
    {
        climbFacadeFancyVTable = climbFacadeTester_FancyVTable_baseclassThatHasSidehopToSwing;
    }
    else
    {
        climbFacadeFancyVTable = climbFacadeTester_FancyVTable_default;
    }
}
ParkourCallbacksForParkourHelpers::ParkourCallbacksForParkourHelpers()
{
    m_Callbacks.ChooseBeforeFiltering_fnp = [](void* userData, SmallArray<AvailableParkourAction*>& actions) -> AvailableParkourAction* {
        return static_cast<ParkourCallbacksForParkourHelpers*>(userData)->ChooseBeforeFiltering(actions);
        };
    m_Callbacks.ChooseAfterSorting_fnp = [](void* userData, SmallArray<AvailableParkourAction*>& actions, AvailableParkourAction* selectedByGame) -> AvailableParkourAction* {
        return static_cast<ParkourCallbacksForParkourHelpers*>(userData)->ChooseAfterSorting(actions, selectedByGame);
        };
    m_Callbacks.m_UserData = this;
    m_Callbacks.m_CallbackPriority = 10.0f;
}
AvailableParkourAction* ParkourCallbacksForParkourHelpers::ChooseBeforeFiltering(SmallArray<AvailableParkourAction*>& actions)
{
    auto [selectedSpindescent, _spindescentIdxBeforeSorting] = SelectBestMatchingMoveIdx_ExtraProcessing_CustomSelection_Spindescent(actions);
    m_SelectedSpindescent = selectedSpindescent;
    return selectedSpindescent;
}
AvailableParkourAction* ParkourCallbacksForParkourHelpers::ChooseAfterSorting(SmallArray<AvailableParkourAction*>& actions, AvailableParkourAction* selectedByGame)
{
    if (g_Config.personalRequests->parkourHelper->canRiseOnLedgeAfterLedgeAssassination)
    {
        if (!selectedByGame && actions.size > 0)
        {
            const bool allTheActionsAreTryingToRiseOnLedge = std::all_of(actions.begin(), actions.end(), [](AvailableParkourAction* action)
                {
                    return (uint64)action->fancyVTable == 0x1439F41C0;
                });
            if (allTheActionsAreTryingToRiseOnLedge)
            {
                return actions[0];
            }
        }
    }
    auto [selectedDiveMove, selectedDiveMoveIdx] = SelectBestMatchingMoveIdx_ExtraProcessing_CustomSelection_DiveMove(actions);

    if (selectedDiveMove)
    {
        // The dive takes the highest priority.
        return selectedDiveMove;
    }
    if (m_SelectedSpindescent)
    {
        // The spindescent takes the second highest priority.
        return m_SelectedSpindescent;
    }
    // If nothing was forced, use the move that the game selected.
    return nullptr;
}
void WhenSettingAllowedAndExpectedRangesForEjectToHang_MakeThemMorePermissive(AllRegisters* params)
{
    auto& action = *(AvailableParkourAction*)params->rdx_;
    action.heightDifferenceMin = -3; // from -0.25f
    action.heightDifferenceMax = 1; // ==
    action.expectedHeightDiff_mb = -2; // from -1
    action.expectedVerticalSpeed_mb = 9; // ==
    action.expectedHorizontalSpeed = 4; // from 4.5f
    action.expectedVerticalDefaultDisplace = -1; // ==
    action.expectedDurationMaxExtension_mb = 1.1f; // from 0.9f
    action.curveAllowedRangeMax = 2; // from 0.5f
    action.curveAllowedRangeMin = -4; // from -2.5f
    action.flt_228 = 1; // from 0?
    action.expectedCurveRangeMin = -2; // from -0.5f
    action.vec240.y = 1.7600001f; // from 1.62f
    action.vec240.z = action.expectedVerticalSpeed_mb * 0.44000003f - 1.8972802f; // from action.expectedVerticalSpeed * 0.35999998f - 1.27008f
}
void WhenSettingAllowedAndExpectedRangesForEjectToHang_ParkourUp_MakeThemMorePermissive(AllRegisters* params)
{
    auto& action = *(AvailableParkourAction*)params->rdx_;
    action.heightDifferenceMin = -12; // from -0.25f
    action.heightDifferenceMax = 1.5f; // from 1
    action.expectedHeightDiff_mb = 1.5f; // from -1
    action.expectedVerticalSpeed_mb = 11; // from 9
    action.expectedHorizontalSpeed = 4; // from 4.5f
    action.expectedVerticalDefaultDisplace = -1; // ==
    action.expectedDurationMin_mb = 0.7f;
    action.expectedDurationMaxExtension_mb = 1.8f; // from 0.9f
    action.curveAllowedRangeMax = 2; // from 0.5f
    action.curveAllowedRangeMin = -4; // from -2.5f
    action.flt_228 = 1; // from 0?
    action.expectedCurveRangeMin = -2; // from -0.5f
    action.vec240.y = 2.8799999f; // from 1.62f
    action.vec240.z = action.expectedVerticalSpeed_mb * 0.71999997f - 5.0803199f; // from action.expectedVerticalSpeed * 0.35999998f - 1.27008f
}
void WhenSettingRTCPTargetAngleForTheSelectedParkourAction_AdjustAngleWhenBackEjectToHang(AllRegisters* params)
{
    /*
    After the WallEjectToHang action has been selected, the value of
        scalar TargetAngle; // 0xd7e18370/3621880688
    RTCP animation variable is updated.
    When performing backeject-to-hang, the TargetAngle is close to 180 or -180 degrees.
    It seems that in the latter case the animation system prefers to use the sideeject
    animation instead. In this case I flip the angle to a positive which seems
    to result in correct backeject animation being chosen.
    */
    if (!IsShouldTryForceWallEjectToHang()) return;
    auto& action = *(AvailableParkourAction*)params->rbx_;
    if (action.GetEnumParkourAction() != EnumParkourAction::wallEjectToHang) return;
    float& calculatedRTCPTargetAngle = (float&)params->XMM0.f0;
    if (calculatedRTCPTargetAngle >= -180.0f && calculatedRTCPTargetAngle < -155.0f)
        calculatedRTCPTargetAngle = -calculatedRTCPTargetAngle;
}
ParkourActionsExtraProcessing::ParkourActionsExtraProcessing()
    : m_GPH(GenericHooksInParkourFiltering::GetSingleton())
{
    m_ParkourCallbacksForParkourHelpers = std::make_unique<ParkourCallbacksForParkourHelpers>();

    {
        PresetScript_CCodeInTheMiddle(0x140135230, 12,
            WhenPerformingScanFor_ClimbFacade_AlsoScanForWallToHang, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
        PresetScript_CCodeInTheMiddle(0x140137A84, 11,
            WhenPerformingScanFor_BackEject_AlsoScanForWallToHang, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

        PresetScript_CCodeInTheMiddle(0x141A4D7CB, 7,
            WhenStartingWallingCycle_ChangeFVTables, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
        PresetScript_CCodeInTheMiddle(0x141A1BA0C, 7,
            WhenStartingFailedWallrunCycle_ChangeFVTables, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
        {
            /*
            The 0x64'th "fancy virtual function" of the ParkourAction
            (SetExpectedDistanceRangesAccordingToParkourMode) is used shortly after
            the ParkourAction is constructed to set the "allowed distance ranges"
            for the tested action.
            Here, I place hooks in the 0x64'th "fancy virtual" of the WallEjectToHang
            (fancyVTable == 1439FD4C0, 0x64'th func == 140140D40)
            in order to expand these "allowed distance ranges" using the values
            used for "sidehop" (see implementation at 140148500)
            and "backeject to ground" (see implementation at 14013E850)
            to make the constructed WallEjectToHang actions less likely to be discarded as unfit.

            UPD: Can't trigger a we2h to the swing bar at [333.85 641.67 7.03],
                 though the actions are created. Maybe they are discarded because too close to the wall?
            */
            PresetScript_CCodeInTheMiddle(0x140140E44, 10,
                WhenSettingAllowedAndExpectedRangesForEjectToHang_MakeThemMorePermissive, 0x140140EDB, false);
            PresetScript_CCodeInTheMiddle(0x140140DCA, 10,
                WhenSettingAllowedAndExpectedRangesForEjectToHang_ParkourUp_MakeThemMorePermissive, 0x140140FED, false);
        }
        PresetScript_CCodeInTheMiddle(0x1401AD835, 7,
            WhenSettingRTCPTargetAngleForTheSelectedParkourAction_AdjustAngleWhenBackEjectToHang, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    }
}
void ParkourActionsExtraProcessing::OnBeforeActivate()
{
    m_GPH->Subscribe(m_ParkourCallbacksForParkourHelpers->m_Callbacks);
    m_Activator_GPHSortAndSelect = m_GPH->RequestGPHSortAndSelect();
}
void ParkourActionsExtraProcessing::OnBeforeDeactivate()
{
    m_Activator_GPHSortAndSelect.reset();
    m_GPH->Unsubscribe(m_ParkourCallbacksForParkourHelpers->m_Callbacks);
}

#include "pch.h"

#include "ParkourDebugging/ParkourDebuggingPatch.h"

#include "vmath/vmath_extra.h"
#include "imgui/imgui_internal.h"

#include "ACU/World.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU/Memory/ACUAllocs.h"
#include "ACU_DefineNativeFunction.h"

#include "MyLog.h"
#include "ImGui3D/ImGui3D.h"
#include "ImGui3D/ImGui3DCustomDraw.h"
#include "ImGui3D/UsefulWireModels.h"
#include "ImGuiCTX.h"
#include "ImGuiConfigUtils.h"
#include "Raycasting/RaycastPicker.h"

#include "AvailableParkourAction.h"
#include "ParkourTester.h"

#include "ParkourLog.h"

// I don't yet know how much there is in common between the structs for different parkour actions,
// So I'm keeping the "AvailableParkourAction" baseclass small.
class ParkourAction_Commonbase : public AvailableParkourAction
{
public:
    char pad_0290[0x2B0-0x290]; //0x0208
}; //Size: 0x02B0
assert_sizeof(ParkourAction_Commonbase, 0x2B0);

DEFINE_LOGGER_CONSOLE_AND_FILE(ParkourLogger, "[Parkour]");

namespace
{
class ParkourDebugWindow
{
public:
    void Draw();
private:
    void DrawSummaryTab();
    void DrawDisplayTab();
    void DrawDetailsTab();
    void DrawActionTypesTab();
    void DrawEnforceTab();


    void ResetWhenStartDrawFrame();

public:
    ParkourDebugWindow();

private:
    auto MakeColumnsForParkourDetails();
    auto MakeColumnsForActionTypes();

public:
    std::optional<float>& GetModWeightForActionType(EnumParkourAction actionType);
private:
    void DrawModWeightSlider(std::optional<float>& modWeight);
private:
    ParkourLog& parkourLog;
    std::shared_ptr<ParkourCycleLogged> latestCycle;

    std::unordered_map<EnumParkourAction, size_t> typesPresentInTheCycle;
    bool m_IsCycleDirty = true;
public:
    std::optional<EnumParkourAction> m_HoveredType;
    ParkourActionLogged* m_HoveredAction = nullptr;

private:
    void SetEnforcedAction(ParkourActionLogged& action);
    struct ContextMenuForAction
    {
        std::shared_ptr<ParkourCycleLogged> m_Cycle;
        ParkourActionLogged* m_Action;
    };
    ImGuiID imid_contextMenuForAction = 0;
    std::optional<ContextMenuForAction> m_ContextMenuForAction;
};

}
class ParkourVisualization : public ImGui3D::CustomDraw::CustomDrawer
{
public:
    static void SetMatrix4fLookAt(Matrix4f& m, const Vector3f& eyePos, const Vector3f& forwardNormalized)
    {
        Vector3f side;
        Vector3f up{ 0, 0, 1 };

        // Side = forward x up
        side = forwardNormalized.crossProduct(up);
        if (side.lengthSq() < EPSILON)
        {
            /*
            By default I try to align the 3D marker's "up" with the positive Z.
            Sometimes the normal direction vector is exactly the same,
            so in that case I have to pick a different default.
            */
            up = { 1, 0, 0 };
            side = forwardNormalized.crossProduct(up);
        }
        side.normalize();

        // Recompute up as: up = side x forward
        up = side.crossProduct(forwardNormalized);

        (Vector3f&)m.data[4 * 0] = side;
        (Vector3f&)m.data[4 * 1] = forwardNormalized;
        (Vector3f&)m.data[4 * 2] = up;

        m.setTranslation(eyePos);
    }
    ParkourDebugWindow m_ParkourDebugWnd;
    virtual void DoDraw() override
    {
        //DoDraw_ManyLocations(m_History_MovesBeforeFiltering);
        //DoDraw_Location(m_History_SelectedMoves);
        m_ParkourDebugWnd.Draw();
        DrawRecentLoggedCycles3D();
    }
    void DrawRecentLoggedCycles3D()
    {
        /*
        Sitting on a beam at [446.54 737.81 8.22]
        and trying to walk straight perpendicularly in low profile
        creates ~1000 actions per frame. Fully visualizing 10 of these cycles
        for a total of 10000+ markers tanks the fps.
        A good place to test drawing performance, I guess.
        The bottleneck seems to be in the calculations within ImGui3D::DrawWireModelTransform().
        */
        World* world = World::GetSingleton();
        if (!world) return;
        const int64 currentTime = world->clockInWorldWithSlowmotion.GetCurrent_RawIntTimestamp();
        auto& parkourLog = ParkourLog::GetSingleton();
        auto& markerModel = GetModel_MarkerWithClearOrientation();
        Matrix4f transform;

        if (parkourLog.m_EnforcedMove.m_Position)
        {
            static auto& modelSphere = GetModel_SphereRadius1();
            SetMatrix4fLookAt(transform, *parkourLog.m_EnforcedMove.m_Position, parkourLog.m_EnforcedMove.m_DirectionFacingOut);
            ImU32 enforcedMoveColor = IM_COL32(0, 255, 0, 255);
            ImGui3D::DrawWireModelTransform(markerModel, transform, 5.0f, enforcedMoveColor);
            ImGui3D::DrawWireModel(modelSphere,
                *parkourLog.m_EnforcedMove.m_Position, 0.5f,
                parkourLog.m_EnforcedMove.m_Radius, enforcedMoveColor
            );
        }
        std::vector<std::shared_ptr<ParkourCycleLogged>> history = parkourLog.GetRecentCycles();

        Matrix4f scaleForOlderCycles = Matrix4f::createScale(0.6f, 0.6f, 0.6f);

        auto DrawCycle = [&](ParkourCycleLogged& cycle, const bool isMostRecent) {
            const float timeElapsed = ConvertRawIntTimestampToFloat(currentTime - cycle.m_Timestamp);

            const unsigned char opacity = isMostRecent ? 255 : 110;

            std::lock_guard _lock{ cycle.m_Mutex };
            auto CalculateFadeFactor_ByTimestamp = [](float timeElapsed) {
                float m_MaxRetainHowLongSecs = 10.0f;
                float fadeFactor = 1 - timeElapsed / m_MaxRetainHowLongSecs;
                if (fadeFactor < 0)         fadeFactor = 0;
                else if (fadeFactor > 1)    fadeFactor = 1;
                return fadeFactor;
                };

            float fadeFactor = CalculateFadeFactor_ByTimestamp(timeElapsed);
            unsigned char redness = int(fadeFactor * 255);
            unsigned char blueness = int((1 - fadeFactor) * 255);
            // The selected one is drawn last to be visible on top of the rest.
            ParkourActionLogged* selectedAction = nullptr;
            auto DrawMarkerForAction = [&](ParkourActionLogged& action) {
                const bool isHovered = m_ParkourDebugWnd.m_HoveredAction == &action || m_ParkourDebugWnd.m_HoveredType == action.m_ActionType;
                const bool isVisible = parkourLog.IsActionShouldBeDisplayed(action) || isHovered;
                if (!isVisible) return;
                const float thickness =
                    isHovered || action.m_IsSelectedInEditor
                    ? 5.0f
                    : 1.0f;
                const bool isTheChosenOne = action.m_IsTheSelectedBestMatch;
                ImU32 color = isHovered
                    ? IM_COL32(255, 255, 255, 255)
                    : IM_COL32(
                        redness,
                        isTheChosenOne ? 255 : 0,
                        blueness,
                        opacity);

                SetMatrix4fLookAt(transform, action.m_LocationDst, action.m_DirDstFacingOut);
                if (!isMostRecent)
                    transform = transform * scaleForOlderCycles;
                ImGui3D::DrawWireModelTransform(markerModel, transform, thickness, color);
                };
            for (auto& action : cycle.m_Actions)
            {
                const bool isTheChosenOne = action->m_IsTheSelectedBestMatch;
                if (isTheChosenOne)
                {
                    selectedAction = action.get();
                    continue;
                }
                DrawMarkerForAction(*action);
            }
            if (selectedAction)
                DrawMarkerForAction(*selectedAction);
            };
        if (parkourLog.m_DisplaySettings.m_ShowMoreThanOneOfTheRecentCycles)
        {
            const bool skipFullyDiscarded = parkourLog.m_DisplaySettings.m_ShowMoreThanOneOfTheRecentCycles_SkipAllDiscarded;
            auto IsCycleShouldBeSkipped = [skipFullyDiscarded](std::shared_ptr<ParkourCycleLogged>& cycle) {
                if (!skipFullyDiscarded) return true;
                const bool isAllDiscarded = cycle->m_Actions.size() && std::all_of(cycle->m_Actions.begin(), cycle->m_Actions.end(), [](decltype(cycle->m_Actions[0])& action) {
                    const bool isDiscarded = action->m_IsDiscarded_immediatelyAfterCreation || action->m_IsDiscarded_becauseFitnessWeightTooLow;
                    return isDiscarded;
                    });
                return !isAllDiscarded;
                };
            size_t numOlderCycles = 10;
            auto olderCycles =
                history
                | std::views::reverse   // From the end
                | std::views::drop(1)   // Skip the most recent one (gets special treatment)
                | std::views::filter(IsCycleShouldBeSkipped)
                | std::views::take(numOlderCycles)
                | std::views::reverse   // Draw 3D from least recent to most recent.
                ;
            for (auto& cycle : olderCycles)
            {
                DrawCycle(*cycle, false);
            }
        }
        if (history.size())
        {
            DrawCycle(*history.back(), true);
        }
    }
    ParkourVisualization() {}
    ~ParkourVisualization() { ImGui3D::CustomDraw::CustomDraw_Unsubscribe(*this); }
} g_ParkourVisualization;
std::vector<EnumParkourAction> g_UnidentifiedParkourActions = {
    EnumParkourAction::unk0,
    EnumParkourAction::stumbleAtEdge_mb,
    EnumParkourAction::leapBeamToVShape,
    EnumParkourAction::unk7,
    EnumParkourAction::passementFromBeam_A,
    EnumParkourAction::passementFromBeam_C,
    EnumParkourAction::passementD,
    EnumParkourAction::unk16,
    EnumParkourAction::unk19,
    EnumParkourAction::unk22,
    EnumParkourAction::unk24,
    EnumParkourAction::unk25,
    EnumParkourAction::unk30,
    EnumParkourAction::swing_2B,
    EnumParkourAction::unk51,
    EnumParkourAction::fromSlopeSlideToHang,
    EnumParkourAction::fromSlopeToWall_side_mb___UNABLE_TO_TRIGGER,
    EnumParkourAction::edgeDropBeforeFreeeFall_3F___UNABLE_TO_TRIGGER,
    EnumParkourAction::unk66,
    EnumParkourAction::unk74,
    EnumParkourAction::backEject_4F,
    EnumParkourAction::unk81,
    EnumParkourAction::unk82,
    EnumParkourAction::unk83,
    EnumParkourAction::unk84,
    EnumParkourAction::unk85,
    EnumParkourAction::choke_mb,
    EnumParkourAction::unk87,
    EnumParkourAction::unk88,
    EnumParkourAction::unk89,
    EnumParkourAction::unk90,
    EnumParkourAction::unk91,
    EnumParkourAction::unk92,
    EnumParkourAction::unk93,
    EnumParkourAction::unk94,
    EnumParkourAction::unk95,
    EnumParkourAction::unk96,
    EnumParkourAction::unk98,
    EnumParkourAction::unk99,
    EnumParkourAction::unk102,
    EnumParkourAction::unk103,
    EnumParkourAction::unk104,
    EnumParkourAction::unk105,
    EnumParkourAction::unk109,
    EnumParkourAction::unk110,
    EnumParkourAction::unk111,
    EnumParkourAction::unk113,
    EnumParkourAction::unk117,
    EnumParkourAction::unk118,
    EnumParkourAction::defenestrateSprintOutOfWindow_122,
    EnumParkourAction::unk124,
};
void LogActionsBeforeFiltering_ConsoleAnd3D(SmallArray<AvailableParkourAction*>& moves)
{
    if (moves.size == 0) return;
    LOG_DEBUG(ParkourLogger, "Num potential actions before filtering: %d"
        , moves.size
    );
    {
        auto movesBeforeFilter_tr = moves | std::views::transform([](auto* action) { return action->GetEnumParkourAction(); });
        std::set<EnumParkourAction> movesBeforeFilter(movesBeforeFilter_tr.begin(), movesBeforeFilter_tr.end());

        if (movesBeforeFilter.size())
        {
            LOG_DEBUG(ParkourLogger,
                "All move types before filtering:\n"
            );
            for (EnumParkourAction actionType : movesBeforeFilter)
            {
                LOG_DEBUG(ParkourLogger,
                    "%3d == %s\n"
                    , actionType
                    , enum_reflection<EnumParkourAction>::GetString(actionType)
                );
            }
        }
    }
    for (AvailableParkourAction* move : moves)
    {
        EnumParkourAction actionType = move->GetEnumParkourAction();
        if (std::find(
            g_UnidentifiedParkourActions.begin(),
            g_UnidentifiedParkourActions.end(),
            actionType) != g_UnidentifiedParkourActions.end())
        {
            LOG_DEBUG(ParkourLogger
                , "[error] Unidentified move: %d == %s"
                , actionType
                , enum_reflection<EnumParkourAction>::GetString(actionType)
            );
        }
    }
}
void LogActionBestMatch_ConsoleAnd3D(AvailableParkourAction& bestMatchMove)
{
    EnumParkourAction actionType = bestMatchMove.GetEnumParkourAction();
    if (actionType == EnumParkourAction::wallrunUpFromGroundFailed_mb)
    {
        int x = 0;
    }
    LOG_DEBUG(ParkourLogger
        , "Selected: %7.2f %7.2f %7.2f (%d == %s)"
        , bestMatchMove.locationAnchorDest.x
        , bestMatchMove.locationAnchorDest.y
        , bestMatchMove.locationAnchorDest.z
        , actionType
        , enum_reflection<EnumParkourAction>::GetString(actionType)
    );
}
std::optional<int> ParkourDebugging_SelectMove(SmallArray<AvailableParkourAction*>& availableParkourActions)
{
    if (availableParkourActions.size != 0
        && std::all_of(
            availableParkourActions.begin(),
            availableParkourActions.end(),
            [](AvailableParkourAction* parkourAction)
            {
                return parkourAction->GetEnumParkourAction() == EnumParkourAction::fromSlopeToWall_side_mb___UNABLE_TO_TRIGGER; // fromSlopeSlideToGrabWall_side
            }))
    {
        return {};
    }
    auto FindIndexForCriterium = [&availableParkourActions](const std::string_view& critName, auto criterium) -> std::optional<int>
        {
            auto foundIt = std::find_if(
                availableParkourActions.begin(),
                availableParkourActions.end(),
                criterium);
            if (foundIt == availableParkourActions.end())
            {
                return {};
            }
            int idx = std::distance(availableParkourActions.begin(), foundIt);
            EnumParkourAction actionType = (*foundIt)->GetEnumParkourAction();
            LOG_DEBUG(ParkourLogger
                , "[error]Selected for %s: idx/size: %d/%hu, %d == %s"
                , critName.data()
                , idx
                , availableParkourActions.size
                , actionType
                , enum_reflection<EnumParkourAction>::GetString(actionType)
            );
            return idx;
        };
    auto DontForceAnything = [](AvailableParkourAction* parkourAction) { return false; };
    auto IsRare = [](AvailableParkourAction* parkourAction)
        {
            return std::find(
                g_UnidentifiedParkourActions.begin(),
                g_UnidentifiedParkourActions.end(),
                parkourAction->GetEnumParkourAction()
            ) != g_UnidentifiedParkourActions.end();
        };
    auto IsTheForcedOne = [](AvailableParkourAction* parkourAction)
        {
            EnumParkourAction actionType = parkourAction->GetEnumParkourAction();
            return
                actionType == EnumParkourAction::fromWallingToHangDownAndTurnCornerOutside
                || actionType == EnumParkourAction::fromHangToWallingUpAndTurnCornerOutside
                || actionType == EnumParkourAction::hangTurnCornerInside
                ;
        };
#define FIND_INDEX_FOR_CRITERIUM(criterium) FindIndexForCriterium(#criterium, criterium)
    auto foundIt = FIND_INDEX_FOR_CRITERIUM(IsRare);
#undef FIND_INDEX_FOR_CRITERIUM
    if (!foundIt)
    {
        return {};
    }
    return *foundIt;
}

namespace ImGui
{
// Make the UI compact because there are so many fields
static void PushStyleCompact()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVarY(ImGuiStyleVar_FramePadding, (float)(int)(style.FramePadding.y * 0.60f));
    ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, (float)(int)(style.ItemSpacing.y * 0.60f));
}

static void PopStyleCompact()
{
    ImGui::PopStyleVar(2);
}
}
void WhenPerformingFinalFilter1OnSortedMoves_ForceTurnInPalaisDeLuxembourgCorners(AllRegisters* params)
{
    DEFINE_GAME_FUNCTION(AvailableParkourAction__FinalFilter1, 0x1401D4DE0, bool, __fastcall, (AvailableParkourAction* p_parkourAction, __m128* a2, uint64 a3, Entity* p_playerEntity));
    AvailableParkourAction* parkourAction = (AvailableParkourAction*)params->rcx_;
    Entity* ent = (Entity*)params->r9_;
    uint64 result = AvailableParkourAction__FinalFilter1(parkourAction, (__m128*)params->rdx_, params->r8_, ent);
    if (ent != ACU::GetPlayer())
    {
        params->GetRAX() = result;
        return;
    }
    SmallArray<ParkourAction_Commonbase*>& moves = *(SmallArray<ParkourAction_Commonbase*>*)params->rsi_;
    if (std::all_of(moves.begin(), moves.end(), [](ParkourAction_Commonbase* action)
        {
            return action->GetEnumParkourAction() == EnumParkourAction::stepDownSmallHeight_mb;
        }))
    {
        result = 1;
    }
    params->GetRAX() = result;
}

static bool CompareOptionalFloats(std::optional<float> a, std::optional<float> b)
{
    return a < b;
}
namespace
{
struct ControlsForParkourType
{
    EnumParkourAction m_ActionType;
    std::optional<float> m_ModWeight;
    bool m_IsSelectedInEditor = false;
    bool m_IsHighlightedInEditor = false;

    ControlsForParkourType(EnumParkourAction actionType) : m_ActionType(actionType) {}
};
using Action_t = std::unique_ptr<ParkourActionLogged>;
template<
    std::invocable<Action_t&>               Draw_fnt
    , std::invocable<Action_t&, Action_t&>  SortPred_fnt
>
struct MoveDetailsColumn
{
    size_t m_ColIdx;
    const char* m_Header;
    Draw_fnt m_Draw;
    SortPred_fnt m_SortPredicate;
};
using ATDesc_t = std::unique_ptr<ControlsForParkourType>;
template<
    std::invocable<ATDesc_t&>               Draw_fnt
    , std::invocable<ATDesc_t&, ATDesc_t&>  SortPred_fnt
>
struct ActionTypesColumn
{
    size_t m_ColIdx;
    const char* m_Header;
    Draw_fnt m_Draw;
    SortPred_fnt m_SortPredicate;
};
template<typename GetAttribute_fnt>
struct SortByAttribute
{
    GetAttribute_fnt m_GetAttributeFn;
    template<typename T>
    bool operator()(T&& a, T&& b)
    {
        return m_GetAttributeFn(a) < m_GetAttributeFn(b);
    }
};
static void for_each_in_tuple(auto&& tuple, auto&& callable)
{
    std::apply([&](auto&&... tupleElem) { ((callable(tupleElem)), ...); }, tuple);
}
const ImVec4 colorTextGreen(0.425f, 0.780f, 0.392f, 1.000f);
const ImVec4 colorTextYellow = ImVec4(1.0f, 0.8f, 0.3f, 1.0f);
const ImVec4 colorTextRed = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
const ImVec4 colorTextAllDiscarded = colorTextRed;
const ImVec4 colorTextSelectedType = colorTextYellow;
const ImVec4 colorTextIsDiscarded = colorTextRed;
const ImVec4 colorTextIsSelected = colorTextYellow;
const ImVec4 colorTextModulate = colorTextGreen;
std::vector<std::unique_ptr<ControlsForParkourType>>& GetRowsForActionTypesTable()
{
    static std::vector<ATDesc_t> result = []() {
        std::vector<ATDesc_t> result;
        for (auto [actionType, strActionType] : enum_reflection<EnumParkourAction>::GetAllPairs())
        {
            result.emplace_back(std::make_unique<ControlsForParkourType>(actionType));
        }
        return result;
        }();
    return result;
}
std::optional<float>& ParkourDebugWindow::GetModWeightForActionType(EnumParkourAction actionType)
{
    std::vector<std::unique_ptr<ControlsForParkourType>>& atDescs = GetRowsForActionTypesTable();
    auto foundIt = std::find_if(atDescs.begin(), atDescs.end(), [&](decltype(atDescs[0])& atDesc) {
        return atDesc->m_ActionType == actionType;
        });
    if (foundIt != atDescs.end())
        return foundIt->get()->m_ModWeight;
    LOG_DEBUG(ParkourLogger,
        "[error]Unknown action type: %d"
        , actionType
    );
    return atDescs.emplace_back(std::make_unique<ControlsForParkourType>(actionType))->m_ModWeight;
}
void ParkourDebugWindow::DrawModWeightSlider(std::optional<float>& modWeight)
{
    ImGui::PushStyleCompact();
    if (!modWeight)
    {
        if (ImGui::Button("Use", ImVec2(-FLT_MIN, 0.0f)))
        {
            modWeight.emplace(1.0f);
        }
    }
    else
    {
        std::optional<ImGuiCTX::PushStyleColor> coloredText;
        const bool isModWeightApplied = modWeight != 1.0f;
        if (modWeight > 1.0f)
            coloredText.emplace(ImGuiCol_Text, colorTextGreen);
        else if (modWeight < 1.0f)
            coloredText.emplace(ImGuiCol_Text, colorTextRed);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::DragFloat("##inpModWeight", &*modWeight, 0.01f, 0.0f, 5.0f);
    }
    ImGui::PopStyleCompact();
}
void ParkourDebugWindow::SetEnforcedAction(ParkourActionLogged& action)
{
    parkourLog.m_EnforcedMove.m_ActionType = action.m_ActionType;
    parkourLog.m_EnforcedMove.m_Position = action.m_LocationDst;
    parkourLog.m_EnforcedMove.m_DirectionFacingOut = action.m_DirDstFacingOut;
}
const char* strId_contextMenuForAction = "ContextMenuForAction";
auto ParkourDebugWindow::MakeColumnsForParkourDetails()
{
    auto DrawCol_Index = [](Action_t& action) { ImGui::Text("%3d", action->m_Index); };
    auto DrawCol_Type = [](Action_t& action) { ImGui::Text("%3d", action->m_ActionType); };
    auto DrawCol_TypeReadable = [this](Action_t& action) {
        ImGuiCTX::PushID _id(action.get());
        static ImGuiTextBuffer buf; buf.resize(0);
        buf.appendf("%s", enum_reflection<EnumParkourAction>::GetString(action->m_ActionType));
        ImGui::Selectable(buf.c_str(), &action->m_IsSelectedInEditor);
        const bool isHovered = ImGui::IsItemHovered();
        if (isHovered)
            m_HoveredAction = action.get();
        {
            buf.resize(0);
            buf.appendf(
                "FancyVTable: %llX\n"
                "%8.3f,%8.3f,%8.3f\n"
                "%8.3f,%8.3f,%8.3f\n"
                "%8.3f,%8.3f,%8.3f\n"
                "%8.3f,%8.3f,%8.3f\n"
                , action->m_FancyVTable
                , action->m_LocationSrc.x, action->m_LocationSrc.y, action->m_LocationSrc.z
                , action->m_DirSrc.x, action->m_DirSrc.y, action->m_DirSrc.z
                , action->m_LocationDst.x
                , action->m_LocationDst.y
                , action->m_LocationDst.z
                , action->m_DirDstFacingOut.x
                , action->m_DirDstFacingOut.y
                , action->m_DirDstFacingOut.z
            );
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(
                    "%s\n"
                    "(Right click to copy to clipboard)"
                    , buf.c_str()
                );
            if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup(imid_contextMenuForAction);
                m_ContextMenuForAction = { latestCycle, action.get() };
            }
            if (ImGui::IsItemClicked(ImGuiMouseButton_Middle))
            {
                SetEnforcedAction(*action);
            }
        }
        };
    auto DrawCol_IsDiscardedImmediately = [](Action_t& action) {
        std::optional<ImGuiCTX::PushStyleColor> coloredText;
        if (action->m_IsDiscarded_immediatelyAfterCreation)
        {
            coloredText.emplace(ImGuiCol_Text, colorTextIsDiscarded);
            ImGui::Text("+");
        }
        };
    auto DrawCol_IsDiscardedBecauseFitnessTooLow = [](Action_t& action) {
        std::optional<ImGuiCTX::PushStyleColor> coloredText;
        if (action->m_IsDiscarded_becauseFitnessWeightTooLow)
        {
            coloredText.emplace(ImGuiCol_Text, colorTextIsDiscarded);
            ImGui::Text("+");
        }
        };
    auto DrawCol_IsChosen = [](Action_t& action) {
        std::optional<ImGuiCTX::PushStyleColor> coloredText;
        if (action->m_IsTheSelectedBestMatch)
        {
            coloredText.emplace(ImGuiCol_Text, colorTextIsSelected);
            ImGui::Text("+");
        }
        };
    auto DrawCol_Collision = [](Action_t& action) { ImGui::Text("%d", action->m_CollisionLayer); };
    auto DrawCol_Fitness = [](Action_t& action) { if (action->m_FitnessWeight) ImGui::Text("%f", *action->m_FitnessWeight); };
    auto DrawCol_DefaultWeight = [](Action_t& action) { if (action->m_DefaultWeight) ImGui::Text("%f", *action->m_DefaultWeight); };
    auto DrawCol_ModWeight = [this](Action_t& action) {
        ImGuiCTX::PushID _id(action.get());
        std::optional<float>& modWeight = GetModWeightForActionType(action->m_ActionType);
        DrawModWeightSlider(modWeight);
        };
    auto DrawCol_TotalWeight = [](Action_t& action) {
        if (!action->m_TotalWeight) return;
        std::optional<ImGuiCTX::PushStyleColor> coloredText;
        if (action->m_UsedModWeight)
        {
            if (*action->m_UsedModWeight > 1)
                coloredText.emplace(ImGuiCol_Text, colorTextGreen);
            else if (*action->m_UsedModWeight < 1)
                coloredText.emplace(ImGuiCol_Text, colorTextRed);
        }
        ImGui::Text("%f", *action->m_TotalWeight);
        };
    enum MoveDetailsColumnsIndices
    {
        Index = 0,
        Type,
        TypeReadable,
        Collision,
        IsDiscardedImmediately,
        Fitness,
        IsDiscardedBecauseFitnessTooLow,
        DefaultWeight,
        ModWeight,
        TotalWeight,
        IsChosen,
    };
#define MDCOL(colId) MoveDetailsColumnsIndices::colId, #colId, DrawCol_##colId
    return std::make_tuple(
            MoveDetailsColumn{ MDCOL(Index), SortByAttribute{[](Action_t& a) { return a->m_Index; }} }
            , MoveDetailsColumn{ MDCOL(Type), SortByAttribute{ [](Action_t& a) { return a->m_ActionType; } } }
            , MoveDetailsColumn{ MDCOL(TypeReadable), SortByAttribute{ [](Action_t& a) { return std::string_view(enum_reflection<EnumParkourAction>::GetString(a->m_ActionType)); } } }
            , MoveDetailsColumn{ MDCOL(Collision), SortByAttribute{ [](Action_t& a) { return a->m_CollisionLayer; } } }
            , MoveDetailsColumn{ MDCOL(IsDiscardedImmediately), SortByAttribute{ [](Action_t& a) { return a->m_IsDiscarded_immediatelyAfterCreation; } } }
            , MoveDetailsColumn{ MDCOL(Fitness), SortByAttribute{ [](Action_t& a) { return a->m_FitnessWeight; } } }
            , MoveDetailsColumn{ MDCOL(IsDiscardedBecauseFitnessTooLow), SortByAttribute{ [](Action_t& a) { return a->m_IsDiscarded_becauseFitnessWeightTooLow; } } }
            , MoveDetailsColumn{ MDCOL(DefaultWeight), SortByAttribute{ [](Action_t& a) { return a->m_DefaultWeight; } } }
            , MoveDetailsColumn{ MDCOL(ModWeight), SortByAttribute{ [this](Action_t& a) { return GetModWeightForActionType(a->m_ActionType); }}}
            , MoveDetailsColumn{ MDCOL(TotalWeight), SortByAttribute{ [](Action_t& a) { return a->m_TotalWeight; } } }
            , MoveDetailsColumn{ MDCOL(IsChosen), SortByAttribute{ [](Action_t& a) { return a->m_IsTheSelectedBestMatch; } } }
        );
#undef MDCOL
};
}
std::optional<float> GetModWeightForAction(AvailableParkourAction& action)
{
    return g_ParkourVisualization.m_ParkourDebugWnd.GetModWeightForActionType(action.GetEnumParkourAction());
}
ParkourDebugWindow::ParkourDebugWindow()
    : parkourLog(ParkourLog::GetSingleton())
{}
void ParkourDebugWindow::DrawSummaryTab()
{
    if (ImGui::IsKeyDown(ImGuiKey_ModAlt))
        ImGui::SetNextFrameWantCaptureMouse(true);
    ImGui::HelpMarker(
        "Hold Alt to block game mouse."
    );
    ImGui::Text("Latest parkour cycle:");
    if (!latestCycle)
    {
        ImGui::Text("Nothing yet.");
        return;
    }
    std::lock_guard _lock{ latestCycle->m_Mutex };
    std::optional<EnumParkourAction> selectedType;
    std::set<EnumParkourAction> actionTypesWithAtLeastOneNondiscarded;
    size_t numNotDiscardedImmediately = 0;
    size_t numNotDiscarded = 0;
    auto allMoves_tr = latestCycle->m_Actions
        | std::views::transform([&](std::unique_ptr<ParkourActionLogged>& action)
            {
                if (action->m_IsTheSelectedBestMatch)
                    selectedType = action->m_ActionType;
                const bool wasDiscardedImmediately = action->m_IsDiscarded_immediatelyAfterCreation;
                if (!wasDiscardedImmediately)
                    numNotDiscardedImmediately++;
                const bool wasDiscarded = action->m_IsDiscarded_immediatelyAfterCreation || action->m_IsDiscarded_becauseFitnessWeightTooLow;
                if (!wasDiscarded)
                {
                    numNotDiscarded++;
                    actionTypesWithAtLeastOneNondiscarded.insert(action->m_ActionType);
                }
                return action->m_ActionType;
            });
    std::set<EnumParkourAction> allMoves(allMoves_tr.begin(), allMoves_tr.end());
    ImGui::Text(
        "Num actions                : %d\n"
        "Num after initial discard  : %d\n"
        "Num with nonzero fitness   : %d\n"
        "Location       : %8.3f,%8.3f,%8.3f\n"
        "Input Direction: %8.3f,%8.3f,%8.3f\n"
        , latestCycle->m_Actions.size()
        , numNotDiscardedImmediately
        , numNotDiscarded
        , latestCycle->m_LocationOfOrigin.x
        , latestCycle->m_LocationOfOrigin.y
        , latestCycle->m_LocationOfOrigin.z
        , latestCycle->m_DirectionOfMovementInputWorldSpace.x
        , latestCycle->m_DirectionOfMovementInputWorldSpace.y
        , latestCycle->m_DirectionOfMovementInputWorldSpace.z
    );

    if (allMoves.size())
    {
        ImGui::Text(
            "All move types: %d"
            , allMoves.size()
        );
        for (const EnumParkourAction actionType : allMoves)
        {
            std::optional<ImGuiCTX::PushStyleColor> selectedActionTextColor;
            const bool isTheSelectedType = actionType == selectedType;
            if (isTheSelectedType) selectedActionTextColor.emplace(ImGuiCol_Text, colorTextSelectedType);
            else if (const bool allMovesOfThisTypeWereDiscarded = !actionTypesWithAtLeastOneNondiscarded.contains(actionType))
                selectedActionTextColor.emplace(ImGuiCol_Text, colorTextAllDiscarded);
            ImGui::Text(
                "%3d == %s"
                , actionType
                , enum_reflection<EnumParkourAction>::GetString(actionType)
            );
        }
    }
}
void ParkourDebugWindow::DrawDisplayTab()
{
    if (!ImGui::IsKeyDown(ImGuiKey_ModAlt))
        ImGui::SetNextFrameWantCaptureMouse(true);
    ImGui::HelpMarker(
        "Hold Alt to _un_block game mouse."
    );
    auto& parkourLog = ParkourLog::GetSingleton();
    parkourLog.DrawDisplayControls();
}
// this implementation assumes normalized quaternion
// converts to Euler angles in 3-2-1 sequence
static Vector3f QuaternionToEulerAngles(Vector4f q) {
    Vector3f angles;

    // roll (x-axis rotation)
    float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.x = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    float sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
    float cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
    angles.y = 2 * std::atan2(sinp, cosp) - M_PI / 2;

    // yaw (z-axis rotation)
    float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.z = std::atan2(siny_cosp, cosy_cosp);

    return angles;
}
static Vector4f EulerToQuaternion(float roll, float pitch, float yaw) // roll (x), pitch (y), yaw (z), angles are in radians
{
    // Abbreviations for the various angular functions

    float cr = cos(roll * 0.5);
    float sr = sin(roll * 0.5);
    float cp = cos(pitch * 0.5);
    float sp = sin(pitch * 0.5);
    float cy = cos(yaw * 0.5);
    float sy = sin(yaw * 0.5);

    Vector4f q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}
static Vector3f GetDirAngles(const Vector3f& dir) {
    Matrix4f m = CreateMatrix4fLookAt(Vector3f(), dir);
    Quatf q = Quatf::fromMatrix(m.getRotation());
    Vector3f dirAsAngles = QuaternionToEulerAngles((Vector4f&)q);
    return dirAsAngles;
}
static Vector3f GetForwardDirFromAngles(const Vector3f& dirAsAngles) {
    Matrix3f rotMat = ((Quatf&)EulerToQuaternion(dirAsAngles.x, dirAsAngles.y, dirAsAngles.z)).rotMatrix();
    return (Vector3f&)rotMat.data[3 * 1];
}
static auto ImGuiEditDirectionAsAngles(Vector3f& dir, auto&& callableOnAngles) {
    Vector3f dirAsAngles = GetDirAngles(dir);
    auto&& result = callableOnAngles(dirAsAngles);
    dir = GetForwardDirFromAngles(dirAsAngles);
    return result;
}
void ParkourDebugWindow::DrawEnforceTab()
{
    if (!ImGui::IsKeyDown(ImGuiKey_ModAlt))
        ImGui::SetNextFrameWantCaptureMouse(true);
    ImGui::HelpMarker(
        "Hold Alt to _un_block game mouse.\n"
        "If a parkour action will be found at the given location (within the given radius)\n"
        "with the given facing direction it will be force selected.\n"
        "Something might happen, might not."
    );
    {
        bool isNeedToReset = false;
        if (ImGui::Button("Clear enforced actions"))
            isNeedToReset = true;
        if (isNeedToReset)
        {
            parkourLog.m_EnforcedMove.m_Position.reset();
            parkourLog.m_EnforcedMove.m_ActionType.reset();
        }
    }
    std::optional<Vector3f> currentFramePos = parkourLog.m_EnforcedMove.m_Position;
    Vector3f currentFrameDir = parkourLog.m_EnforcedMove.m_DirectionFacingOut;
    static RaycastPickerModal picker;
    picker.Pick("Pick location",
        [&](const MyRaycastSuccessfulHit& hit) {
            parkourLog.m_EnforcedMove.m_Position = hit.m_HitLocation;
            parkourLog.m_EnforcedMove.m_DirectionFacingOut = hit.m_Normal;
        },
        [&](const MyRaycastSuccessfulHit& hit) {
            RaycastPickerModal::DefaultOnEveryHit(hit);
            currentFramePos = hit.m_HitLocation;
            currentFrameDir = hit.m_Normal;
        },
        RaycastPickerModal::DefaultOnNoHit
    );
    if (currentFramePos)
    {
        ImGui::SameLine();
        bool isNeedToReset = false;
        if (ImGui::Button("Clear"))
            isNeedToReset = true;
        bool isChanged = false;
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Position :"); ImGui::SameLine();
        isChanged |= ImGui::DragFloat3("##pos", *currentFramePos, 0.01f);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Direction:"); ImGui::SameLine();
        isChanged |= ImGuiEditDirectionAsAngles(currentFrameDir, [](Vector3f& dirAsAngles) {
            return ImGui::DragFloat3("##dir", dirAsAngles, 0.01f);
            });
        if (isChanged)
        {
            parkourLog.m_EnforcedMove.m_Position = currentFramePos;
            parkourLog.m_EnforcedMove.m_DirectionFacingOut = currentFrameDir;
        }
        if (isNeedToReset)
            parkourLog.m_EnforcedMove.m_Position.reset();
    }
    else
    {
        ImGui::Dummy(ImVec2{ 0, ImGui::GetFrameHeight() * 2 + ImGui::GetStyle().ItemSpacing.y });
    }
    ImGui::DragFloat("Radius", &parkourLog.m_EnforcedMove.m_Radius, 0.0025f, 0.1f, 5.0f);
}
namespace ImGui
{
static void DrawRowsWithClipper(int items_count, std::invocable<int> auto&& callableForRowWithIdx)
{
    // Demonstrate using clipper for large vertical lists
    ImGuiListClipper clipper;
    clipper.Begin(items_count);
    while (clipper.Step())
    {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
        {
            ImGui::TableNextRow();
            callableForRowWithIdx(row);
        }
    }
}
}
void ParkourDebugWindow::DrawDetailsTab()
{
    ImGui::HelpMarker(
        "Hold Alt to block game mouse.\n"
        "Actions selected with left click are displayed in thicker lines."
    );
    if (ImGui::IsKeyDown(ImGuiKey_ModAlt))
        ImGui::SetNextFrameWantCaptureMouse(true);
    if (!latestCycle) return;
    std::lock_guard _lock{ latestCycle->m_Mutex };
    auto columns = MakeColumnsForParkourDetails();
    const size_t numColumns = std::tuple_size_v<decltype(columns)>;
    ImGuiTableFlags table_flags =
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_ScrollY
        | ImGuiTableFlags_Resizable
        | ImGuiTableFlags_RowBg
        | ImGuiTableFlags_Sortable
        | ImGuiTableFlags_SizingFixedFit
        ;
    imid_contextMenuForAction = ImGui::GetID(strId_contextMenuForAction);
    if (m_ContextMenuForAction)
    {
        auto& actions = m_ContextMenuForAction->m_Cycle->m_Actions;
        auto foundIt = std::find_if(actions.begin(), actions.end(), [&](auto&& a) {
            return a.get() == m_ContextMenuForAction->m_Action;
            });
        if (foundIt == actions.end())
        {
            m_ContextMenuForAction.reset();
        }
        else
        {
            ParkourActionLogged& action = **foundIt;
            if (ImGuiCTX::Popup _contextMenuForAction{ strId_contextMenuForAction })
            {
                static ImGuiTextBuffer buf;
                buf.resize(0);
                buf.appendf(
                    "FancyVTable: %llX\n"
                    "%8.3f,%8.3f,%8.3f\n"
                    "%8.3f,%8.3f,%8.3f\n"
                    "%8.3f,%8.3f,%8.3f\n"
                    "%8.3f,%8.3f,%8.3f\n"
                    , action.m_FancyVTable
                    , action.m_LocationSrc.x, action.m_LocationSrc.y, action.m_LocationSrc.z
                    , action.m_DirSrc.x, action.m_DirSrc.y, action.m_DirSrc.z
                    , action.m_LocationDst.x
                    , action.m_LocationDst.y
                    , action.m_LocationDst.z
                    , action.m_DirDstFacingOut.x
                    , action.m_DirDstFacingOut.y
                    , action.m_DirDstFacingOut.z
                );
                ImGui::Text(buf.c_str());
                ImGui::Separator();
                if (ImGui::MenuItem("Copy to clipboard"))
                    ImGui::SetClipboardText(buf.c_str());
                if (ImGui::MenuItem("Enforce"))
                {
                    SetEnforcedAction(action);
                }
            }
        }
    }
    if (ImGui::BeginTable("Moves details", (int)numColumns, table_flags))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        for_each_in_tuple(columns, [](auto&& detailsColumn) {
            ImGui::TableSetupColumn(detailsColumn.m_Header);
            });
        ImGui::TableHeadersRow();
        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs())
        {
            if ((latestCycle->m_IsSortingDirty || sortSpecs->SpecsDirty) && sortSpecs->SpecsCount > 0)
            {
                sortSpecs->SpecsDirty = false;
                latestCycle->m_IsSortingDirty = false;
                const ImGuiTableColumnSortSpecs& primSort = sortSpecs->Specs[0];
                using Act_t = std::unique_ptr<ParkourActionLogged>;
                auto ApplySort = [&]<std::invocable<Act_t&, Act_t&> Pred>(Pred && predicate) {
                    if (primSort.SortDirection == ImGuiSortDirection_None) return;
                    const bool isAscending = primSort.SortDirection == ImGuiSortDirection_Ascending;
                    auto& acts = latestCycle->m_Actions;
                    using Act_t = std::unique_ptr<ParkourActionLogged>;
                    auto adjustedPredicate = [&](Act_t& a, Act_t& b) { return isAscending ? predicate(a, b) : predicate(b, a); };
                    std::sort(acts.begin(), acts.end(), adjustedPredicate);
                };
                for_each_in_tuple(columns, [&](auto&& detailsColumn) {
                    if (detailsColumn.m_ColIdx != primSort.ColumnIndex) return;
                    ApplySort(detailsColumn.m_SortPredicate);
                    });
            }
        }
        auto DrawRow = [&](std::unique_ptr<ParkourActionLogged>& action) {
            for_each_in_tuple(columns, [&](auto&& detailsColumn) {
                ImGui::TableSetColumnIndex((int)detailsColumn.m_ColIdx);
                detailsColumn.m_Draw(action);
                });
            };
        ImGui::DrawRowsWithClipper((int)latestCycle->m_Actions.size(), [&](int row) {
            std::unique_ptr<ParkourActionLogged>& action = latestCycle->m_Actions[(size_t)row];
            DrawRow(action);
            });
        ImGui::EndTable();
    }
}
auto ParkourDebugWindow::MakeColumnsForActionTypes()
{
    auto GetNumUsesInCycle = [this](EnumParkourAction actionType) -> size_t {
        if (!typesPresentInTheCycle.contains(actionType)) return 0;
        return typesPresentInTheCycle[actionType];
        };
    auto IsActionTypeShownAsDisabled = [this, GetNumUsesInCycle](EnumParkourAction actionType) {
        if (typesPresentInTheCycle.empty()) return false;
        return GetNumUsesInCycle(actionType) <= 0;
        };
    enum ActionTypeColumnsIndices
    {
        TypeInt = 0,
        TypeReadable,
        UsesInCycle,
        ModWeight,
    };
    auto DrawCol_TypeInt = [](ATDesc_t& atDesc) {
        ImGui::Text("%3d", atDesc->m_ActionType);
        };
    auto DrawCol_UsesInCycle = [GetNumUsesInCycle](ATDesc_t& atDesc) {
        size_t numUses = GetNumUsesInCycle(atDesc->m_ActionType);
        if (numUses > 0)
            ImGui::Text("%d", numUses);
        };
    auto DrawCol_TypeReadable = [this, IsActionTypeShownAsDisabled](ATDesc_t& atDesc) {
        const char* readable = enum_reflection<EnumParkourAction>::GetString(atDesc->m_ActionType);
        const bool showAsDisabled = IsActionTypeShownAsDisabled(atDesc->m_ActionType);
        ImVec4 colTextDisabled = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
        std::optional<ImGuiCTX::PushStyleColor> disabledText;
        if (showAsDisabled)
            disabledText.emplace(ImGuiCol_Text, colTextDisabled);
        ImGui::Selectable(readable);
        if (ImGui::IsItemHovered())
        {
            m_HoveredType = atDesc->m_ActionType;
        }
        };
    auto DrawCol_ModWeight = [this](ATDesc_t& atDesc) {
        ImGuiCTX::PushID _id(&atDesc);
        DrawModWeightSlider(atDesc->m_ModWeight);
        };

#define ATCOL(colId) ActionTypeColumnsIndices::colId, #colId, DrawCol_##colId
    return std::make_tuple(
        ActionTypesColumn{ ATCOL(TypeInt), SortByAttribute{ [](ATDesc_t& a) { return a->m_ActionType; }} }
        , ActionTypesColumn{ ATCOL(TypeReadable), SortByAttribute{ [](ATDesc_t& a) { return std::string_view(enum_reflection<EnumParkourAction>::GetString(a->m_ActionType)); } } }
        , ActionTypesColumn{ ATCOL(UsesInCycle), SortByAttribute{ [GetNumUsesInCycle](ATDesc_t& a) { return GetNumUsesInCycle(a->m_ActionType); }} }
        , ActionTypesColumn{ ATCOL(ModWeight), SortByAttribute{ [](ATDesc_t& a) { return a->m_ModWeight; } } }
        );
#undef ATCOL
}
void ParkourDebugWindow::DrawActionTypesTab()
{
    if (ImGui::IsKeyDown(ImGuiKey_ModAlt))
        ImGui::SetNextFrameWantCaptureMouse(true);
    std::optional<std::lock_guard<decltype(latestCycle->m_Mutex)>> _lock;
    typesPresentInTheCycle.clear();
    if (latestCycle)
    {
        _lock.emplace(latestCycle->m_Mutex);
        for (Action_t& action : latestCycle->m_Actions)
        {
            typesPresentInTheCycle[action->m_ActionType]++;
        }
    }
    auto&& columns = MakeColumnsForActionTypes();
    std::vector<std::unique_ptr<ControlsForParkourType>>& rows = GetRowsForActionTypesTable();
    size_t numColumns = std::tuple_size_v<std::remove_reference_t<decltype(columns)>>;
    ImGuiTableFlags table_flags =
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_ScrollY
        | ImGuiTableFlags_Resizable
        | ImGuiTableFlags_RowBg
        | ImGuiTableFlags_Sortable
        | ImGuiTableFlags_SizingFixedFit
        ;
    if (ImGui::BeginTable("Action types", (int)numColumns, table_flags))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        for_each_in_tuple(columns, [](auto&& actionTypeCol) {
            ImGui::TableSetupColumn(actionTypeCol.m_Header);
            });
        ImGui::TableHeadersRow();
        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs())
        {
            if ((m_IsCycleDirty || sortSpecs->SpecsDirty) && sortSpecs->SpecsCount > 0)
            {
                sortSpecs->SpecsDirty = false;
                m_IsCycleDirty = false;
                const ImGuiTableColumnSortSpecs& primSort = sortSpecs->Specs[0];
                using Elem_t = ATDesc_t;
                auto ApplySort = [&]<std::invocable<Elem_t&, Elem_t&> Pred>(Pred && predicate) {
                    if (primSort.SortDirection == ImGuiSortDirection_None) return;
                    const bool isAscending = primSort.SortDirection == ImGuiSortDirection_Ascending;
                    auto adjustedPredicate = [&](auto&& a, auto&& b) { return isAscending ? predicate(a, b) : predicate(b, a); };
                    std::ranges::sort(rows, adjustedPredicate);
                };
                for_each_in_tuple(columns, [&](auto&& detailsColumn) {
                    if (detailsColumn.m_ColIdx != primSort.ColumnIndex) return;
                    ApplySort(detailsColumn.m_SortPredicate);
                    });
            }
        }
        auto DrawRow = [&](ATDesc_t& atDesc) {
            for_each_in_tuple(columns, [&](auto&& atColumn) {
                ImGui::TableSetColumnIndex((int)atColumn.m_ColIdx);
                atColumn.m_Draw(atDesc);
                });
            };
        ImGui::DrawRowsWithClipper((int)rows.size(), [&](int row) {
            auto& atDesc = rows[(size_t)row];
            DrawRow(atDesc);
            });
        ImGui::EndTable();
    }
}
void ParkourDebugWindow::ResetWhenStartDrawFrame()
{
    auto newLatestCycle = parkourLog.GetLatestLoggedParkourCycle();
    if (latestCycle != newLatestCycle)
        m_IsCycleDirty = true;
    latestCycle = std::move(newLatestCycle);
    m_HoveredType.reset();
    m_HoveredAction = nullptr;
}
void ParkourDebugWindow::Draw()
{
    ResetWhenStartDrawFrame();
    if (latestCycle)
    {
        Matrix4f tr;
        tr.setRotation(MakeRotationAlignZWithVector(latestCycle->m_DirectionOfMovementInputWorldSpace));
        tr = Matrix4f::createTranslation(latestCycle->m_LocationOfOrigin) * tr;
        ImGui3D::DrawWireModelTransform(ImGui3D::GetArrowModel(), tr);
    }
    ImGui::GetIO().MouseDrawCursor = true;
    ImGui::SetNextWindowPos(ImVec2{ 70, 350 }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2{ 1000, 520 }, ImGuiCond_FirstUseEver);
    static float opacity = 0.74f;
    ImGuiCTX::PushStyleColor _wndBg{ ImGuiCol_WindowBg, ImVec4(0, 0, 0, opacity)};
    if (ImGuiCTX::Window _wnd{ "Parkour Debug", 0, ImGuiWindowFlags_MenuBar })
    {
        if (ImGuiCTX::MenuBar _menuBar{})
        {
            ImGui::SetNextItemWidth(100.0f);
            ImGui::SliderFloat("Opacity", &opacity, 0.05f, 1.0f);
        }
        if (latestCycle)
            ImGui::Text(
                "Timestamp: %llu\n"
                , latestCycle->m_Timestamp
            );
        else
            ImGui::NewLine();
        if (ImGuiCTX::TabBar _tb{ "_pdtb" }) {
            if (ImGuiCTX::Tab _tabSummary{ "Summary" }) {
                DrawSummaryTab();
            }
            if (ImGuiCTX::Tab _tabMoveDetails{ "Details" }) {
                DrawDetailsTab();
            }
            if (ImGuiCTX::Tab _tabDisplay{ "Action types" }) {
                DrawActionTypesTab();
            }
            if (ImGuiCTX::Tab _tabDisplay{ "Display" }) {
                DrawDisplayTab();
            }
            if (ImGuiCTX::Tab _tabDisplay{ "Enforce" }) {
                DrawEnforceTab();
            }
        }
    }
}
AvailableParkourAction* ParkourCallbacksForParkourDebug::ChooseAfterSorting(SmallArray<AvailableParkourAction*>& allActionsSorted, AvailableParkourAction* selectionAfterGameAndCallbacks)
{
    auto& parkourLog = ParkourLog::GetSingleton();
    if (AvailableParkourAction* bestAction = parkourLog.m_EnforcedMove.FindMatchingAction(allActionsSorted))
    {
        selectionAfterGameAndCallbacks = bestAction;
        return bestAction;
    }
    return nullptr;
}
ParkourCallbacksForParkourDebug::ParkourCallbacksForParkourDebug()
{
    m_Callbacks.ChooseAfterSorting_fnp = [](void* userData, SmallArray<AvailableParkourAction*>& actions, AvailableParkourAction* selectedByGame) -> AvailableParkourAction* {
        return static_cast<ParkourCallbacksForParkourDebug*>(userData)->ChooseAfterSorting(actions, selectedByGame);
        };
    m_Callbacks.m_UserData = this;
    m_Callbacks.m_CallbackPriority = 10000.0f;
    m_Callbacks.m_Name = "ParkourDebug_EnforceMove";
}
bool g_IsParkourDebuggingActive = false;
ParkourDebuggingPatch::ParkourDebuggingPatch()
    : m_GPH(GenericHooksInParkourFiltering::GetSingleton())
{}
void ParkourDebuggingPatch::OnBeforeActivate()
{
    m_GPH->Subscribe(m_Callbacks.m_Callbacks);
    m_Activator_GPHCreation = m_GPH->RequestGPHCreation();
    m_Activator_GPHSortAndSelect = m_GPH->RequestGPHSortAndSelect();
    g_IsParkourDebuggingActive = true;
    ImGui3D::CustomDraw::CustomDraw_Subscribe(g_ParkourVisualization);
}
void ParkourDebuggingPatch::OnBeforeDeactivate()
{
    g_IsParkourDebuggingActive = false;
    m_Activator_GPHCreation.reset();
    m_Activator_GPHSortAndSelect.reset();
    m_GPH->Unsubscribe(m_Callbacks.m_Callbacks);
    ImGui3D::CustomDraw::CustomDraw_Unsubscribe(g_ParkourVisualization);
}

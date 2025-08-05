#include "pch.h"

#include "ParkourDebugging/ParkourDebuggingPatch.h"

#include "vmath/vmath_extra.h"

#include "ACU/World.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU/Memory/ACUAllocs.h"
#include "ACU_DefineNativeFunction.h"

#include "MyLog.h"
#include "ImGui3D/ImGui3D.h"
#include "ImGui3D/ImGui3DCustomDraw.h"
#include "ImGuiCTX.h"
#include "ImGuiConfigUtils.h"

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
struct History_Location_Entry
{
    Vector3f m_Location;
    float m_Timestamp;
};
class History_Location
{
public:
    float m_MaxRetainHowLongSecs = 10.0f;
    size_t m_MaxRetainNum = 1024;
    size_t m_MaxDisplayNum = 10;
    std::vector<History_Location_Entry> m_History;
    void Draw()
    {
        ImGuiTextBuffer buf;
        for (size_t i = 0; i < m_History.size(); i++)
        {
            buf.clear();
            buf.appendf("ParkourPos %d", m_History.size() - i - 1);
            ImGui3D::DrawLocationNamed(m_History[i].m_Location, buf.c_str());
        }
    }
    void Add(const Vector3f& location, float timestamp)
    {
        size_t curSize = m_History.size();
        if (curSize >= m_MaxRetainNum)
        {
            m_History.erase(m_History.begin(), m_History.begin() + (curSize - m_MaxRetainNum + 1));
        }
        m_History.push_back(History_Location_Entry{ location, timestamp });
    }
    History_Location()
    {
        m_History.reserve(1024);
    }
};
struct History_ManyLocations_Entry
{
    std::vector<Vector3f> m_ManyLocations;
    float m_Timestamp;
};
class History_ManyLocations
{
public:
    float m_MaxRetainHowLongSecs = 10.0f;
    size_t m_MaxRetainNum = 1024;
    size_t m_MaxDisplayNum = 10;
    std::vector<History_ManyLocations_Entry> m_History;
    void Add(std::vector<Vector3f>&& locations, float timestamp)
    {
        size_t curSize = m_History.size();
        if (curSize >= m_MaxRetainNum)
        {
            m_History.erase(m_History.begin(), m_History.begin() + (curSize - m_MaxRetainNum + 1));
        }
        m_History.push_back(History_ManyLocations_Entry{ std::move(locations), timestamp });
    }
    History_ManyLocations()
    {
        m_History.reserve(1024);
    }
};
ImGui3D::ImGuiWireModel& GetModel_Camera()
{
    using namespace ImGui3D;
    static ImGuiWireModel camModel = {
        // Points.
        {
            {-0.154847f, 0.342506f, -0.088164f},
            {0.154847f, 0.342506f, -0.088164f},
            {-0.154847f, 0.342506f, 0.088164f},
            {0.154847f, 0.342506f, 0.088164f},
            {0.000000f, 0.000000f, 0.000000f},
            {-0.109504f, 0.341621f, 0.104011f},
            {0.109225f, 0.341621f, 0.104011f},
            {-0.000139f, 0.341621f, 0.218292f},
        },
        // Edges
        {
            {2, 0, IM_COL32(255, 0, 0, 255)},
            {0, 1, IM_COL32(255, 0, 0, 255)},
            {1, 3, IM_COL32(255, 0, 0, 255)},
            {3, 2, IM_COL32(255, 0, 0, 255)},
            {0, 4, IM_COL32(255, 0, 0, 255)},
            {3, 4, IM_COL32(255, 0, 0, 255)},
            {4, 1, IM_COL32(255, 0, 0, 255)},
            {4, 2, IM_COL32(255, 0, 0, 255)},
            {7, 5, IM_COL32(255, 0, 0, 255)},
            {5, 6, IM_COL32(255, 0, 0, 255)},
            {6, 7, IM_COL32(255, 0, 0, 255)},
        } };
    return camModel;
}
ImGui3D::ImGuiWireModel& GetModel_MarkerWithClearOrientation() {
    // The Blender's Camera model has a clearly readable origin, direction and orientation, works well for visualization.
    return GetModel_Camera();
}
class ParkourVisualization : public ImGui3D::CustomDraw::CustomDrawer
{
public:
    History_Location m_History_SelectedMoves;
    History_ManyLocations m_History_MovesBeforeFiltering;
    static void DoDraw_Location(History_Location& historySingleLoc)
    {
        auto* world = World::GetSingleton();
        if (!world) return;
        auto& crossModel = ImGui3D::GetCrossModel();
        auto& history = historySingleLoc.m_History;
        size_t curSize = history.size();
        size_t numDisplayed = std::min(curSize, historySingleLoc.m_MaxDisplayNum);
        size_t firstIdx = curSize - numDisplayed;
        size_t pastLastIdx = curSize;
        auto CalculateFadeFactor_ByIndex = [&](size_t i)
            {
                return (float)(i + 1) / numDisplayed;
            };
        auto CalculateFadeFactor_ByTimestamp = [&](size_t i)
            {
                auto& entry = history[i];
                const float currentTime = world->clockInWorldWithSlowmotion.GetCurrentTimeFloat();
                const float timeElapsed = currentTime - entry.m_Timestamp;
                float fadeFactor = 1 - timeElapsed / historySingleLoc.m_MaxRetainHowLongSecs;
                if (fadeFactor < 0) fadeFactor = 0;
                else if (fadeFactor > 1) fadeFactor = 1;
                return fadeFactor;
            };
        for (size_t i = firstIdx; i < pastLastIdx; i++)
        {
            float fadeFactor = CalculateFadeFactor_ByTimestamp(i);
            ImU32 color = IM_COL32(
                int(fadeFactor * 255),
                255,
                int((1 - fadeFactor) * 255),
                255
            );
            ImGui3D::DrawWireModel(crossModel, history[i].m_Location, 1.0f, 0.2f, color);
        }
    }
    static void DoDraw_ManyLocations(History_ManyLocations& historyManyLocs)
    {
        World* world = World::GetSingleton();
        if (!world) return;
        auto& crossModel = ImGui3D::GetCrossModel();
        auto& history = historyManyLocs.m_History;
        size_t curSize = history.size();
        size_t numDisplayed = std::min(curSize, historyManyLocs.m_MaxDisplayNum);
        size_t firstIdx = curSize - numDisplayed;
        size_t pastLastIdx = curSize;
        auto CalculateFadeFactor_ByIndex = [&](size_t i)
            {
                return (float)(i + 1) / numDisplayed;
            };
        auto CalculateFadeFactor_ByTimestamp = [&](size_t i)
            {
                auto& entry = history[i];
                const float currentTime = world->clockInWorldWithSlowmotion.GetCurrentTimeFloat();
                const float timeElapsed = currentTime - entry.m_Timestamp;
                float fadeFactor = 1 - timeElapsed / historyManyLocs.m_MaxRetainHowLongSecs;
                if (fadeFactor < 0) fadeFactor = 0;
                else if (fadeFactor > 1) fadeFactor = 1;
                return fadeFactor;
            };
        for (size_t i = firstIdx; i < pastLastIdx; i++)
        {
            float fadeFactor = CalculateFadeFactor_ByTimestamp(i);
            ImU32 color = IM_COL32(
                int(fadeFactor * 255),
                0,
                int((1 - fadeFactor) * 255),
                255
            );
            for (size_t j = 0; j < history[i].m_ManyLocations.size(); j++)
            {
                ImGui3D::DrawWireModel(crossModel, history[i].m_ManyLocations[j], 1.0f, 0.2f, color);
            }
        }
    }
    static void SetMatrix4fLookAt(Matrix4f& m, const Vector3f& eyePos, const Vector3f& forwardNormalized, const Vector3f& upDir)
    {
        Vector3f side, up;

        up = upDir;

        // Side = forward x up
        side = forwardNormalized.crossProduct(up);
        side.normalize();

        // Recompute up as: up = side x forward
        up = side.crossProduct(forwardNormalized);

        (Vector3f&)m.data[4 * 0] = side;
        (Vector3f&)m.data[4 * 1] = forwardNormalized;
        (Vector3f&)m.data[4 * 2] = up;

        m.setTranslation(eyePos);
    }
    virtual void DoDraw() override
    {
        //DoDraw_ManyLocations(m_History_MovesBeforeFiltering);
        //DoDraw_Location(m_History_SelectedMoves);
        void DrawParkourDebugWindow(); DrawParkourDebugWindow();
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
        std::vector<std::shared_ptr<ParkourCycleLogged>> history = parkourLog.GetRecentCycles();

        auto& markerModel = GetModel_MarkerWithClearOrientation();
        Matrix4f transform;
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
                const bool isHovered = action.m_IsHighlightedInEditor;
                const bool isVisible = parkourLog.IsActionShouldBeDisplayed(action) || isHovered;
                if (!isVisible) return;
                const float thickness =
                    action.m_IsHighlightedInEditor || action.m_IsSelectedInEditor
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

                SetMatrix4fLookAt(transform, action.m_LocationDst, action.m_DirDstFacingOut, Vector3f(0, 0, 1));
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
    std::vector<Vector3f> entries;
    entries.reserve(moves.size);
    for (uint16 i = 0; i < moves.size; i++)
    {
        AvailableParkourAction* parkourMove = moves[i];
        entries.push_back((Vector3f&)parkourMove->locationAnchorDest);
    }
    g_ParkourVisualization.m_History_MovesBeforeFiltering.Add(
        std::move(entries)
        , World::GetSingleton()->clockInWorldWithSlowmotion.GetCurrentTimeFloat()
    );
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
        , bestMatchMove.locationAnchorSrc.x
        , bestMatchMove.locationAnchorSrc.y
        , bestMatchMove.locationAnchorSrc.z
        , actionType
        , enum_reflection<EnumParkourAction>::GetString(actionType)
    );
    //ImGui3D::DrawLocationNamed((Vector3f&)bestMatchMove->locationAnchorSrc, "locationAnchorSrc");
    //ImGui3D::DrawLocationNamed((Vector3f&)bestMatchMove->locationAnchorDest, "locationAnchorDest");
    //ImGui3D::DrawLocationNamed((Vector3f&)bestMatchMove->handsLocationTo_right_mb, "handsLocationTo_right_mb");
    //ImGui3D::DrawLocationNamed((Vector3f&)bestMatchMove->handsLocationTo_left_mb, "handsLocationTo_left_mb");
    g_ParkourVisualization.m_History_SelectedMoves.Add(
        (Vector3f&)bestMatchMove.locationAnchorDest
        , World::GetSingleton()->clockInWorldWithSlowmotion.GetCurrentTimeFloat()
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
template<std::invocable<Action_t&> GetAttribute_fnt>
struct SortByAttribute
{
    GetAttribute_fnt&& m_GetAttributeFn;
    bool operator()(Action_t& a, Action_t& b)
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
std::optional<float>& GetModWeightForActionType(EnumParkourAction actionType)
{
    static std::unordered_map<EnumParkourAction, std::optional<float>> modWeightByActionType;
    modWeightByActionType.try_emplace(actionType, std::optional<float>());
    return modWeightByActionType.find(actionType)->second;
}
auto DrawCol_ModWeight = [](Action_t& action) {
    ImGuiCTX::PushID _id(action.get());
    static ImVec2 cell_padding(0.0f, 0.0f);
    ImGuiCTX::PushStyleVar noPaddingAroundInput(ImGuiStyleVar_CellPadding, cell_padding);
    ImGui::PushStyleCompact();
    std::optional<float>& modWeight = GetModWeightForActionType(action->m_ActionType);
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
    };
//MoveDetailsColumn{ MDCOL(ModWeight), SortByAttribute{ [](Action_t& a) { return GetModWeightForActionType(a->m_ActionType); } } };
static auto MakeColumnsForParkourDetails()
{
    auto DrawCol_Index = [](Action_t& action) { ImGui::Text("%3d", action->m_Index); };
    auto DrawCol_Type = [](Action_t& action) { ImGui::Text("%3d", action->m_ActionType); };
    auto DrawCol_TypeReadable = [](Action_t& action) {
        ImGuiCTX::PushID _id(action.get());
        static ImGuiTextBuffer buf; buf.resize(0);
        buf.appendf("%s", enum_reflection<EnumParkourAction>::GetString(action->m_ActionType));
        ImGui::Selectable(buf.c_str(), &action->m_IsSelectedInEditor);
        const bool isHovered = ImGui::IsItemHovered();
        action->m_IsHighlightedInEditor = isHovered;
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
            ImGui::SetItemTooltip(
                "%s\n"
                "(Right click to copy to clipboard)"
                , buf.c_str()
            );
            if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) ImGui::SetClipboardText(buf.c_str());
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
    auto DrawCol_Fitness = [](Action_t& action) { if (action->m_FitnessWeight) ImGui::Text("%f", *action->m_FitnessWeight); };
    auto DrawCol_DefaultWeight = [](Action_t& action) { if (action->m_DefaultWeight) ImGui::Text("%f", *action->m_DefaultWeight); };
    auto DrawCol_TotalWeight = [](Action_t& action) { if (action->m_TotalWeight) ImGui::Text("%f", *action->m_TotalWeight); };
    enum MoveDetailsColumnsIndices
    {
        Index = 0,
        Type,
        TypeReadable,
        IsDiscardedImmediately,
        Fitness,
        IsDiscardedBecauseFitnessTooLow,
        DefaultWeight,
        TotalWeight,
        IsChosen,
    };
#define MDCOL(colId) MoveDetailsColumnsIndices::colId, #colId, DrawCol_##colId
    return std::make_tuple(
            MoveDetailsColumn{ MDCOL(Index), SortByAttribute{[](Action_t& a) { return a->m_Index; }} }
            , MoveDetailsColumn{ MDCOL(Type), SortByAttribute{ [](Action_t& a) { return a->m_ActionType; } } }
            , MoveDetailsColumn{ MDCOL(TypeReadable), SortByAttribute{ [](Action_t& a) { return std::string_view(enum_reflection<EnumParkourAction>::GetString(a->m_ActionType)); } } }
            , MoveDetailsColumn{ MDCOL(IsDiscardedImmediately), SortByAttribute{ [](Action_t& a) { return a->m_IsDiscarded_immediatelyAfterCreation; } } }
            , MoveDetailsColumn{ MDCOL(Fitness), SortByAttribute{ [](Action_t& a) { return a->m_FitnessWeight; } } }
            , MoveDetailsColumn{ MDCOL(IsDiscardedBecauseFitnessTooLow), SortByAttribute{ [](Action_t& a) { return a->m_IsDiscarded_becauseFitnessWeightTooLow; } } }
            , MoveDetailsColumn{ MDCOL(DefaultWeight), SortByAttribute{ [](Action_t& a) { return a->m_DefaultWeight; } } }
            , MoveDetailsColumn{ MDCOL(TotalWeight), SortByAttribute{ [](Action_t& a) { return a->m_TotalWeight; } } }
            , MoveDetailsColumn{ MDCOL(IsChosen), SortByAttribute{ [](Action_t& a) { return a->m_IsTheSelectedBestMatch; } } }
        );
#undef MDCOL
};
}
void DrawParkourDebugWindow()
{
    ParkourLog& parkourLog = ParkourLog::GetSingleton();
    std::shared_ptr<ParkourCycleLogged> latestCycle = parkourLog.GetLatestLoggedParkourCycle();
    if (latestCycle)
    {
        Matrix4f tr;
        tr.setRotation(MakeRotationAlignZWithVector(latestCycle->m_DirectionOfMovementInputWorldSpace));
        tr = Matrix4f::createTranslation(latestCycle->m_LocationOfOrigin) * tr;
        ImGui3D::DrawWireModelTransform(ImGui3D::GetArrowModel(), tr);
    }
    ImGui::GetIO().MouseDrawCursor = true;
    ImGui::SetNextWindowPos(ImVec2{ 170, 290 }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2{ 362, 520 }, ImGuiCond_FirstUseEver);
    static float opacity = 0.74f;
    ImGuiCTX::PushStyleColor _wndBg{ ImGuiCol_WindowBg, ImVec4(0, 0, 0, opacity)};
    if (ImGuiCTX::Window _wnd{ "Parkour Debug", 0, ImGuiWindowFlags_MenuBar })
    {
        if (ImGuiCTX::MenuBar _menuBar{})
        {
            ImGui::SetNextItemWidth(100.0f);
            ImGui::SliderFloat("Opacity", &opacity, 0.05f, 1.0f);
        }
        auto DrawSummaryTab = [&]() {
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
                "Timestamp: %llu\n"
                "Num actions                : %d\n"
                "Num after initial discard  : %d\n"
                "Num with nonzero fitness   : %d\n"
                "Location       : %8.3f,%8.3f,%8.3f\n"
                "Input Direction: %8.3f,%8.3f,%8.3f\n"
                , latestCycle->m_Timestamp
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
            };
        auto DrawDisplayTab = [&]() {
            if (!ImGui::IsKeyDown(ImGuiKey_ModAlt))
                ImGui::SetNextFrameWantCaptureMouse(true);
            ImGui::HelpMarker(
                "Hold Alt to _un_block game mouse."
            );
            auto& parkourLog = ParkourLog::GetSingleton();
            parkourLog.DrawDisplayControls();
            };
        auto DrawDetailsTab = [&]() {
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
            if (ImGui::BeginTable("Moves details", numColumns, table_flags))
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
                        auto ApplySort = [&]<std::invocable<Act_t&, Act_t&> Pred>(Pred&& predicate) {
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
                // Demonstrate using clipper for large vertical lists
                ImGuiListClipper clipper;
                clipper.Begin((int)latestCycle->m_Actions.size());
                while (clipper.Step())
                {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                    {
                        ImGui::TableNextRow();
                        std::unique_ptr<ParkourActionLogged>& action = latestCycle->m_Actions[(size_t)row];
                        DrawRow(action);
                    }
                }
                ImGui::EndTable();
            }
            };
        if (ImGuiCTX::TabBar _tb{ "_pdtb" }) {
            if (ImGuiCTX::Tab _tabSummary{ "Summary" }) {
                DrawSummaryTab();
            }
            if (ImGuiCTX::Tab _tabMoveDetails{ "Details" }) {
                DrawDetailsTab();
            }
            if (ImGuiCTX::Tab _tabDisplay{ "Display" }) {
                DrawDisplayTab();
            }
        }
    }
}
bool g_IsParkourDebuggingActive = false;
ParkourDebuggingPatch::ParkourDebuggingPatch() {}
void ParkourDebuggingPatch::OnBeforeActivate()
{
    m_Activator_GPHCreation = GenericHooksInParkourFiltering::GetSingleton().RequestGPHCreation();
    m_Activator_GPHSortAndSelect = GenericHooksInParkourFiltering::GetSingleton().RequestGPHSortAndSelect();
    g_IsParkourDebuggingActive = true;
    ImGui3D::CustomDraw::CustomDraw_Subscribe(g_ParkourVisualization);
}
void ParkourDebuggingPatch::OnBeforeDeactivate()
{
    m_Activator_GPHCreation.reset();
    m_Activator_GPHSortAndSelect.reset();
    g_IsParkourDebuggingActive = false;
    ImGui3D::CustomDraw::CustomDraw_Unsubscribe(g_ParkourVisualization);
}

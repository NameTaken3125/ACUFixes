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

#include "AvailableParkourAction.h"
#include "ParkourTester.h"

class Entity;
class EntityGroup;
struct PlayerRefInParkourAction
{
    SharedPtrNew<Entity>* shared_player;
    uint64 qword_8;
    uint32 dword_10;
    char pad_14[4];
};
assert_sizeof(PlayerRefInParkourAction, 0x18);
// I don't yet know how much there is in common between the structs for different parkour actions,
// So I'm keeping the "AvailableParkourAction" baseclass small.
class ParkourAction_Commonbase : public AvailableParkourAction
{
public:
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
    PlayerRefInParkourAction shared_player; //0x0100
    char pad_0118[96]; //0x0118
    SharedPtrNew<EntityGroup>* shared_entityGroupWithVisualCpntsOnly_178_canBeEmpty; //0x0178
    Vector4f vec180; //0x0180
    Vector4f vec190; //0x0190
    Vector4f vec1A0; //0x01A0
    char pad_01B0[84]; //0x01B0
    float simpleFitness; //0x0204
    char pad_0208[168]; //0x0208
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
    virtual void DoDraw() override
    {
        DoDraw_ManyLocations(m_History_MovesBeforeFiltering);
        DoDraw_Location(m_History_SelectedMoves);
        void DrawParkourDebugWindow(); DrawParkourDebugWindow();
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
DEFINE_GAME_FUNCTION(AvailableParkourAction__FinalFilter1, 0x1401D4DE0, bool, __fastcall, (AvailableParkourAction* p_parkourAction, __m128* a2, uint64 a3, Entity* p_playerEntity));
DEFINE_GAME_FUNCTION(AvailableParkourAction__FinalFilter2, 0x1401D3000, bool, __fastcall, (AvailableParkourAction* p_parkourAction, Entity* p_playerEntity, __int64 a3));
DEFINE_GAME_FUNCTION(ConstructParkourAction_A, 0x1401CC990, ParkourAction_Commonbase*, __fastcall, (EnumParkourAction p_actionEnum, __int64 a2, __m128* a3, __m128* p_movementVecWorld_mb, int a5, char a6, __int64 a7, __int64 a8));
DEFINE_GAME_FUNCTION(ConstructParkourAction_B, 0x1401D1FC0, ParkourAction_Commonbase*, __fastcall, (EnumParkourAction p_actionEnum, __int64 a2, __m128* a3, __m128* p_movementVecWorld_mb, int a5, char a6, __int64 a7, __int64 a8, Entity* p_player, uint64 p_currentLedge_mb));
DEFINE_GAME_FUNCTION(AvailableParkourAction__InitializePlayerRef, 0x14015A570, void, __fastcall, (PlayerRefInParkourAction* p_playerRefOut, Entity* a2));
// Used for a SmallArray of plain types, where constructors/destructors don't matter.
DEFINE_GAME_FUNCTION(SmallArray_POD__RemoveGeneric, 0x142725F00, void, __fastcall, (void* smallArray, int p_idx, unsigned int p_elemSize));

void WhenPerformingFinalFilter1OnSortedMoves_ForceTurnInPalaisDeLuxembourgCorners(AllRegisters* params)
{
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

class ParkourActionLogged
{
public:
    // Used for identification within the log, will definitely become invalid after the "parkour cycle" finishes execution.
    // Note that this address probably isn't a good enough identifier on its own.
    // The actions can be discarded immediately after creation, so it's possible
    // that several different actions will be created at the same address within the same cycle.
    // Though I guess no more than one can be not-discarded-immediately-after-creation.
    AvailableParkourAction* m_ActionPtr;
    ParkourActionLogged(AvailableParkourAction& action, size_t indexInOrderOfCreationInCycle)
        : m_ActionPtr(&action)
        , m_ActionType(action.GetEnumParkourAction())
        , m_LocationSrc((Vector3f&)action.locationAnchorSrc)
        , m_LocationDst((Vector3f&)action.locationAnchorDest)
        , m_DirSrc((Vector3f&)action.direction_20)
        , m_DirDst((Vector3f&)action.direction_40)
        , m_Index(indexInOrderOfCreationInCycle)
    {}
    size_t m_Index; // To keep track of the order of creation.

    EnumParkourAction m_ActionType;
    Vector3f m_LocationSrc; // I think these are the same in every action of the parkour cycle.
    Vector3f m_DirSrc;      // I think these are the same in every action of the parkour cycle.
    Vector3f m_LocationDst;
    Vector3f m_DirDst;
    bool m_IsDiscarded_immediatelyAfterCreation = false;
    std::optional<float> m_FitnessWeight;
    bool m_IsDiscarded_becauseFitnessWeightTooLow = false;
    std::optional<float> m_DefaultWeight;
    std::optional<float> m_TotalWeight;
    std::optional<bool> m_ResultOfFinalFilter1;
    std::optional<bool> m_ResultOfFinalFilter2;
    bool m_IsTheSelectedBestMatch = false;
};
class ParkourLog;
class ParkourCycleLogged
{
public:
    uint64 m_Timestamp;
    ParkourCycleLogged(uint64 timestamp) : m_Timestamp(timestamp), m_IsDisabled(false) {}
    std::vector<std::unique_ptr<ParkourActionLogged>> m_Actions;
    std::mutex m_Mutex;
    bool m_IsSortingDirty = true;

    Vector3f m_LocationOfOrigin;
    Vector3f m_DirectionOfMovementInputWorldSpace;

    void LogActionInitialCreation(AvailableParkourAction& newAction, bool isDiscarded_immediatelyAfterCreation);
    void LogActionsBeforeFiltering(SmallArray<ParkourAction_Commonbase*>& allActionsBeforeFiltering);
    void LogActionBeforeFiltering(AvailableParkourAction& action, float fitness, bool isDiscarded_becauseFitnessWeightTooLow);
    void LogActionWeights(AvailableParkourAction& action, float defaultWeight, float totalWeight);
    void LogActionFinalFilter1(AvailableParkourAction& action, bool resultOfFinalFilter1);
    void LogActionFinalFilter2(AvailableParkourAction& action, bool resultOfFinalFilter2);
    void LogActionWhenReturningBestMatch(AvailableParkourAction& bestMatchMove);

private:
    size_t m_IndexOfNextLoggedAction = 0; // To keep track of the order of creation.
    ParkourActionLogged& GetOrMakeRecordForAction(AvailableParkourAction& action)
    {
        std::lock_guard _lock{ m_Mutex };
        m_IsSortingDirty = true; // Assume something gets modified after this call.
        for (auto& record : m_Actions)
        {
            if (record->m_ActionPtr != &action) continue;
            if (record->m_IsDiscarded_immediatelyAfterCreation) continue;
            return *record;
        }
        return *m_Actions.emplace_back(std::make_unique<ParkourActionLogged>(action, m_IndexOfNextLoggedAction++));
    }
    ParkourActionLogged& MakeRecordForAction(AvailableParkourAction& action)
    {
        std::lock_guard _lock{ m_Mutex };
        return *m_Actions.emplace_back(std::make_unique<ParkourActionLogged>(action, m_IndexOfNextLoggedAction++));
    }
private:
    bool m_IsDisabled;
    ParkourCycleLogged() : m_Timestamp(-1), m_IsDisabled(true) {}
    friend ParkourLog;
};
void ParkourCycleLogged::LogActionInitialCreation(AvailableParkourAction& action, bool isDiscarded_immediatelyAfterCreation)
{
    if (m_IsDisabled) return;
    ParkourActionLogged& recordForAction = MakeRecordForAction(action);
    recordForAction.m_IsDiscarded_immediatelyAfterCreation = isDiscarded_immediatelyAfterCreation;
}
void ParkourCycleLogged::LogActionBeforeFiltering(AvailableParkourAction& action, float fitness, bool isDiscarded_becauseFitnessWeightTooLow)
{
    if (m_IsDisabled) return;
    ParkourActionLogged& recordForAction = this->GetOrMakeRecordForAction(action);
    recordForAction.m_FitnessWeight = fitness;
    recordForAction.m_IsDiscarded_becauseFitnessWeightTooLow = isDiscarded_becauseFitnessWeightTooLow;
}
void ParkourCycleLogged::LogActionWeights(AvailableParkourAction& action, float defaultWeight, float totalWeight)
{
    if (m_IsDisabled) return;
    ParkourActionLogged& recordForAction = this->GetOrMakeRecordForAction(action);
    recordForAction.m_DefaultWeight = defaultWeight;
    recordForAction.m_TotalWeight = totalWeight;
}
void ParkourCycleLogged::LogActionsBeforeFiltering(SmallArray<ParkourAction_Commonbase*>& allActionsBeforeFiltering)
{
    if (m_IsDisabled) return;
    SmallArray<ParkourAction_Commonbase*>& moves = allActionsBeforeFiltering;
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
    for (ParkourAction_Commonbase* move : moves)
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
        ParkourAction_Commonbase* parkourMove = moves[i];
        entries.push_back((Vector3f&)parkourMove->locationAnchorDest);
    }
    g_ParkourVisualization.m_History_MovesBeforeFiltering.Add(
        std::move(entries)
        , World::GetSingleton()->clockInWorldWithSlowmotion.GetCurrentTimeFloat()
    );
}
void ParkourCycleLogged::LogActionFinalFilter1(AvailableParkourAction& action, bool resultOfFinalFilter1)
{
    if (m_IsDisabled) return;
    ParkourActionLogged& record = this->GetOrMakeRecordForAction(action);
    record.m_ResultOfFinalFilter1 = resultOfFinalFilter1;
}
void ParkourCycleLogged::LogActionFinalFilter2(AvailableParkourAction& action, bool resultOfFinalFilter2)
{
    if (m_IsDisabled) return;
    ParkourActionLogged& record = this->GetOrMakeRecordForAction(action);
    record.m_ResultOfFinalFilter2 = resultOfFinalFilter2;
}
void ParkourCycleLogged::LogActionWhenReturningBestMatch(AvailableParkourAction& bestMatchMove)
{
    if (m_IsDisabled) return;
    ParkourActionLogged& record = this->GetOrMakeRecordForAction(bestMatchMove);
    record.m_IsTheSelectedBestMatch = true;
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
class ParkourLog
{
public:
    std::shared_ptr<ParkourCycleLogged> m_LatestParkourCycle;
    std::shared_ptr<ParkourCycleLogged> GetCurrentLoggedParkourCycle();
    // Doesn't construct a new cycle.
    std::shared_ptr<ParkourCycleLogged> GetLatestLoggedParkourCycle() { return m_LatestParkourCycle; }

public:
    static ParkourLog& GetSingleton() { static ParkourLog singleton; return singleton; }
private:
    std::shared_ptr<ParkourCycleLogged> m_DisabledDummyCycle;
    ParkourLog() : m_DisabledDummyCycle(new ParkourCycleLogged()) {}
};
std::shared_ptr<ParkourCycleLogged> ParkourLog::GetCurrentLoggedParkourCycle()
{
    const bool isParkourLogDisabled = false;
    if (isParkourLogDisabled) return m_DisabledDummyCycle;

    uint64 currentTimestamp = World::GetSingleton()->clockInWorldWithSlowmotion.currentTimestamp;
    if (!m_LatestParkourCycle || m_LatestParkourCycle->m_Timestamp != currentTimestamp)
        m_LatestParkourCycle = std::make_shared<ParkourCycleLogged>(currentTimestamp);
    return m_LatestParkourCycle;
}
// The returned shared_ptr is non-null.
std::shared_ptr<ParkourCycleLogged> GetCurrentLoggedParkourCycle()
{
    return ParkourLog::GetSingleton().GetCurrentLoggedParkourCycle();
}
static bool CompareOptionalFloats(std::optional<float> a, std::optional<float> b)
{
    return a < b;
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
    if (ImGuiCTX::Window _wnd{ "Parkour Debug" })
    {
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
                "Location   : %8.3f,%8.3f,%8.3f\n"
                , latestCycle->m_Timestamp
                , latestCycle->m_Actions.size()
                , numNotDiscardedImmediately
                , numNotDiscarded
                , latestCycle->m_LocationOfOrigin.x
                , latestCycle->m_LocationOfOrigin.y
                , latestCycle->m_LocationOfOrigin.z
            );

            if (allMoves.size())
            {
                ImGui::Text(
                    "All move types: %d"
                    , allMoves.size()
                );
                const ImVec4 colorSelectedType = ImVec4(1.0f, 0.8f, 0.3f, 1.0f);
                const ImVec4 colorAllDiscarded = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                for (const EnumParkourAction actionType : allMoves)
                {
                    std::optional<ImGuiCTX::PushStyleColor> selectedActionTextColor;
                    const bool isTheSelectedType = actionType == selectedType;
                    if (isTheSelectedType) selectedActionTextColor.emplace(ImGuiCol_Text, colorSelectedType);
                    else if (const bool allMovesOfThisTypeWereDiscarded = !actionTypesWithAtLeastOneNondiscarded.contains(actionType))
                        selectedActionTextColor.emplace(ImGuiCol_Text, colorAllDiscarded);
                    ImGui::Text(
                        "%3d == %s"
                        , actionType
                        , enum_reflection<EnumParkourAction>::GetString(actionType)
                    );
                }
            }
            };
        auto DrawDetailsTab = [&]() {
            if (!latestCycle) return;
            std::lock_guard _lock{ latestCycle->m_Mutex };
            const ImVec4 colIsSelected = ImVec4(1.0f, 0.8f, 0.3f, 1.0f);
            const ImVec4 colIsDiscarded = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
            enum MoveDetailsColumns
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

                MoveDetailsColumns_COUNT,
            };
            if (ImGui::BeginTable("Moves details", MoveDetailsColumns_COUNT,
                ImGuiTableFlags_Borders
                | ImGuiTableFlags_Resizable
                | ImGuiTableFlags_RowBg
                | ImGuiTableFlags_Sortable
                | ImGuiTableFlags_SizingFixedFit
            )) {
                ImGui::TableSetupColumn("Index");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("TypeReadable");
                ImGui::TableSetupColumn("IsDiscardedImmediately");
                ImGui::TableSetupColumn("Fitness");
                ImGui::TableSetupColumn("IsDiscardedBecauseFitnessTooLow");
                ImGui::TableSetupColumn("DefaultWeight");
                ImGui::TableSetupColumn("TotalWeight");
                ImGui::TableSetupColumn("IsChosen");
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
                        switch (static_cast<MoveDetailsColumns>(primSort.ColumnIndex))
                        {
                        case MoveDetailsColumns::Index:
                            ApplySort([](Act_t& a, Act_t& b) { return a->m_Index < b->m_Index; });
                            break;
                        case MoveDetailsColumns::Type:
                            ApplySort([](Act_t& a, Act_t& b) { return a->m_ActionType < b->m_ActionType; });
                            break;
                        case MoveDetailsColumns::TypeReadable:
                            ApplySort([](Act_t& a, Act_t& b) {
                                return std::string_view(enum_reflection<EnumParkourAction>::GetString(a->m_ActionType))
                                    < std::string_view(enum_reflection<EnumParkourAction>::GetString(b->m_ActionType));
                                    });
                            break;
                        case MoveDetailsColumns::IsDiscardedImmediately:
                            ApplySort([](Act_t& a, Act_t& b) { return a->m_IsDiscarded_immediatelyAfterCreation < b->m_IsDiscarded_immediatelyAfterCreation; });
                            break;
                        case MoveDetailsColumns::Fitness:
                            ApplySort([](Act_t& a, Act_t& b) { return CompareOptionalFloats(a->m_FitnessWeight, b->m_FitnessWeight); });
                            break;
                        case MoveDetailsColumns::IsDiscardedBecauseFitnessTooLow:
                            ApplySort([](Act_t& a, Act_t& b) { return a->m_IsDiscarded_becauseFitnessWeightTooLow < b->m_IsDiscarded_becauseFitnessWeightTooLow; });
                            break;
                        case MoveDetailsColumns::DefaultWeight:
                            ApplySort([](Act_t& a, Act_t& b) { return CompareOptionalFloats(a->m_DefaultWeight, b->m_DefaultWeight); });
                            break;
                        case MoveDetailsColumns::TotalWeight:
                            ApplySort([](Act_t& a, Act_t& b) { return CompareOptionalFloats(a->m_TotalWeight, b->m_TotalWeight); });
                            break;
                        case MoveDetailsColumns::IsChosen:
                            ApplySort([](Act_t& a, Act_t& b) { return a->m_IsTheSelectedBestMatch < b->m_IsTheSelectedBestMatch; });
                            break;
                        }
                    }
                }
                for (auto& action : latestCycle->m_Actions)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(MoveDetailsColumns::Index);
                    ImGui::Text("%3d", action->m_Index);
                    ImGui::TableSetColumnIndex(MoveDetailsColumns::Type);
                    ImGui::Text("%3d", action->m_ActionType);
                    ImGui::TableSetColumnIndex(MoveDetailsColumns::TypeReadable);
                    ImGui::Text("%s", enum_reflection<EnumParkourAction>::GetString(action->m_ActionType));
                    {
                        static ImGuiTextBuffer buf; buf.resize(0);
                        buf.appendf(
                            "%8.3f,%8.3f,%8.3f\n"
                            "%8.3f,%8.3f,%8.3f\n"
                            "%8.3f,%8.3f,%8.3f\n"
                            "%8.3f,%8.3f,%8.3f\n"
                            , action->m_LocationSrc.x, action->m_LocationSrc.y, action->m_LocationSrc.z
                            , action->m_DirSrc.x, action->m_DirSrc.y, action->m_DirSrc.z
                            , action->m_LocationDst.x
                            , action->m_LocationDst.y
                            , action->m_LocationDst.z
                            , action->m_DirDst.x
                            , action->m_DirDst.y
                            , action->m_DirDst.z
                        );
                        ImGui::SetItemTooltip(buf.c_str());
                        if (ImGui::IsItemClicked()) ImGui::SetClipboardText(buf.c_str());
                    }
                    ImGui::TableSetColumnIndex(MoveDetailsColumns::IsDiscardedImmediately);
                    {
                        std::optional<ImGuiCTX::PushStyleColor> coloredText;
                        if (action->m_IsDiscarded_immediatelyAfterCreation)
                        {
                            coloredText.emplace(ImGuiCol_Text, colIsDiscarded);
                            ImGui::Text("+");
                        }
                    }
                    ImGui::TableSetColumnIndex(MoveDetailsColumns::Fitness);
                    if (action->m_FitnessWeight)
                    {
                        ImGui::Text("%f", *action->m_FitnessWeight);
                    }
                    ImGui::TableSetColumnIndex(MoveDetailsColumns::IsDiscardedBecauseFitnessTooLow);
                    {
                        std::optional<ImGuiCTX::PushStyleColor> coloredText;
                        if (action->m_IsDiscarded_becauseFitnessWeightTooLow)
                        {
                            coloredText.emplace(ImGuiCol_Text, colIsDiscarded);
                            ImGui::Text("+");
                        }
                    }
                    ImGui::TableSetColumnIndex(MoveDetailsColumns::DefaultWeight);
                    if (action->m_DefaultWeight)
                    {
                        ImGui::Text("%f", *action->m_DefaultWeight);
                    }
                    ImGui::TableSetColumnIndex(MoveDetailsColumns::TotalWeight);
                    if (action->m_TotalWeight)
                    {
                        ImGui::Text("%f", *action->m_TotalWeight);
                    }
                    ImGui::TableSetColumnIndex(MoveDetailsColumns::IsChosen);
                    {
                        std::optional<ImGuiCTX::PushStyleColor> coloredText;
                        if (action->m_IsTheSelectedBestMatch)
                        {
                            coloredText.emplace(ImGuiCol_Text, colIsSelected);
                            ImGui::Text("+");
                        }
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
        }
    }
}
bool CreateParkourActionAndPerformInitialTestIfFits_A_FullReplacement(
    EnumParkourAction actionType,
    uint64 a2,
    __m128* a3,
    __m128* p_movementVecWorld_mb,
    float a5, int a6, char a7, uint64 a8,
    Entity* p_player,
    uint64 p_currentLedge_mb,
    ParkourAction_Commonbase** const p_newAction_out,
    float a12,
    float p_epsilon_mb)
{
    ParkourAction_Commonbase* newAction = ConstructParkourAction_A(actionType, a2, a3, p_movementVecWorld_mb, a6, a7, a8, p_currentLedge_mb);
    *p_newAction_out = newAction;
    if (!newAction) return false;
    if (p_player)
        AvailableParkourAction__InitializePlayerRef(&newAction->shared_player, p_player);
    GET_AND_CAST_FANCY_FUNC(*newAction, ParkourActionKnownFancyVFuncs::Set2FloatsAfterCreation)(newAction, a12, p_epsilon_mb);
    bool isActionFits = GET_AND_CAST_FANCY_FUNC(*newAction, ParkourActionKnownFancyVFuncs::InitialTestIfActionFits)(
        newAction,
        a2,
        a3,
        p_movementVecWorld_mb,
        a5,
        a6,
        a8,
        p_player,
        p_currentLedge_mb);
    bool isDiscarded = !isActionFits;
    GetCurrentLoggedParkourCycle()->LogActionInitialCreation(*newAction, isDiscarded);
    if (isDiscarded)
    {
        newAction->Unk008_Destroy(0);
        ACU::Memory::ACUDeallocateBytes((byte*)newAction);
        *p_newAction_out = nullptr;
        return false;
    }
    return true;
}
bool CreateParkourActionAndPerformInitialTestIfFits_B_FullReplacement(
    EnumParkourAction actionType,
    uint64 a2,
    __m128* a3,
    __m128* p_movementVecWorld_mb,
    float a5, int a6, char a7, uint64 a8, uint64 a9,
    Entity* p_player,
    uint64 p_currentLedge_mb,
    ParkourAction_Commonbase** const p_newAction_out,
    float a13,
    float p_epsilon_mb)
{
    ParkourAction_Commonbase* newAction = ConstructParkourAction_B(actionType, a2, a3, p_movementVecWorld_mb, a6, a7, a8, a9, p_player, p_currentLedge_mb);
    *p_newAction_out = newAction;
    if (!newAction) return false;
    if (p_player)
        AvailableParkourAction__InitializePlayerRef(&newAction->shared_player, p_player);
    GET_AND_CAST_FANCY_FUNC(*newAction, ParkourActionKnownFancyVFuncs::Set2FloatsAfterCreation)(newAction, a13, p_epsilon_mb);
    bool isActionFits = GET_AND_CAST_FANCY_FUNC(*newAction, ParkourActionKnownFancyVFuncs::InitialTestIfActionFits)(
        newAction,
        a2,
        a3,
        p_movementVecWorld_mb,
        a5,
        a6,
        a9,
        p_player,
        p_currentLedge_mb);
    bool isDiscarded = !isActionFits;
    GetCurrentLoggedParkourCycle()->LogActionInitialCreation(*newAction, isDiscarded);
    if (isDiscarded)
    {
        newAction->Unk008_Destroy(0);
        ACU::Memory::ACUDeallocateBytes((byte*)newAction);
        *p_newAction_out = nullptr;
        return false;
    }
    return true;
}
int SortAndSelectBestMatchingAction_FullReplacement(
    ParkourTester* parkourTester,
    __m128* p_locationOfOrigin,
    uint64 a3,
    __m128* p_directionOfMovementInputWorldSpace,
    float a5,
    int a6,
    char a7,
    __int64 a8,
    SmallArray<ParkourAction_Commonbase*>& p_parkourSensorsResults)
{
    std::shared_ptr<ParkourCycleLogged> currentCycle = GetCurrentLoggedParkourCycle();
    currentCycle->LogActionsBeforeFiltering(p_parkourSensorsResults);
    currentCycle->m_LocationOfOrigin = *(Vector3f*)p_locationOfOrigin;
    currentCycle->m_DirectionOfMovementInputWorldSpace = *(Vector3f*)p_directionOfMovementInputWorldSpace;

    if (p_parkourSensorsResults.size > 0)
    {
        for (int i = 0; i < p_parkourSensorsResults.size; i++)
        {
            AvailableParkourAction* action = p_parkourSensorsResults[i];
            float fitness = GET_AND_CAST_FANCY_FUNC(*action, ParkourActionKnownFancyVFuncs::GetFitness)(action);
            bool isDiscarded_becauseFitnessWeightTooLow = fabsf(fitness) <= 0.0000099999997;
            currentCycle->LogActionBeforeFiltering(*action, fitness, isDiscarded_becauseFitnessWeightTooLow);
            if (isDiscarded_becauseFitnessWeightTooLow)
            {
                SmallArray_POD__RemoveGeneric(&p_parkourSensorsResults, i--, 8u);
            }
        }
        // Some of the elements of the array are removed, sooo...
        // where are the deallocations of the elements?
        // Maybe it's just a copied array of nonowning pointers.
    }
    {
        // Idk what this call is, doesn't seem to do anything.
        __m128 smthOut;
        GET_AND_CAST_FANCY_FUNC(*parkourTester, ParkourTesterKnownFancyVFuncs::ParkourTester_FancyVFunc_0x16)(&smthOut, *parkourTester, p_locationOfOrigin, p_directionOfMovementInputWorldSpace);
    }
    {
        struct ActionWeights
        {
            float fitness, defaultWeight, totalWeight;
        };
        auto GetActionWeights = [](ParkourTester& parkourTester, AvailableParkourAction& action) -> ActionWeights {
            float fitness = GET_AND_CAST_FANCY_FUNC(action, ParkourActionKnownFancyVFuncs::GetFitness)(&action);
            float defaultWeight = GET_AND_CAST_FANCY_FUNC(parkourTester, ParkourTesterKnownFancyVFuncs::GetDefaultWeightForAction)(parkourTester, action);
            float totalWeight = fitness * defaultWeight;

            return { fitness, defaultWeight, totalWeight };
            };
        auto SortByDescendingTotalFitness = [&](AvailableParkourAction* actionFirst, AvailableParkourAction* action2nd) {
            ActionWeights weights1st = GetActionWeights(*parkourTester, *actionFirst);
            ActionWeights weights2nd = GetActionWeights(*parkourTester, *action2nd);
            currentCycle->LogActionWeights(*actionFirst, weights1st.defaultWeight, weights1st.totalWeight);
            currentCycle->LogActionWeights(*action2nd, weights2nd.defaultWeight, weights2nd.totalWeight);
            return weights1st.totalWeight > weights2nd.totalWeight;
            };
        std::sort(p_parkourSensorsResults.begin(), p_parkourSensorsResults.end(), SortByDescendingTotalFitness);
    }
    int selectedBestMatch = -1; // "-1" means "no move is selected".
    for (uint16 i = 0; i < p_parkourSensorsResults.size; i++)
    {
        AvailableParkourAction* action = p_parkourSensorsResults[i];
        const bool isPassedFinalFilter1 = AvailableParkourAction__FinalFilter1(action, p_locationOfOrigin, a8, parkourTester->entity);
        currentCycle->LogActionFinalFilter1(*p_parkourSensorsResults[i], isPassedFinalFilter1);
        if (!isPassedFinalFilter1) continue;
        const bool isPassedFinalFilter2 = AvailableParkourAction__FinalFilter2(action, parkourTester->entity, a8);
        currentCycle->LogActionFinalFilter2(*p_parkourSensorsResults[i], isPassedFinalFilter2);
        if (!isPassedFinalFilter2) continue;

        // The action with the greatest "totalWeight" that passes these preceding two tests
        // is selected.
        selectedBestMatch = i;
        {
            // Original implementation makes another call to "GetFitness()" and does nothing with the result.
            // Are there any side effects to this? Idk.
            //float __fitness = GET_AND_CAST_FANCY_FUNC(*action, ParkourActionKnownFancyVFuncs::GetFitness)(action);
        }
        currentCycle->LogActionWhenReturningBestMatch(*p_parkourSensorsResults[i]);
        break;
    }
    int result = selectedBestMatch;
    return result;
}
ParkourDebuggingPatch::ParkourDebuggingPatch()
{
    //uintptr_t whenPerformingFinalFilter1OnSortedMoves = 0x140133D16;
    //PresetScript_CCodeInTheMiddle(whenPerformingFinalFilter1OnSortedMoves, 5,
    //    WhenPerformingFinalFilter1OnSortedMoves_ForceTurnInPalaisDeLuxembourgCorners, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);

    // Most "action types" are created from this call.
    PresetScript_ReplaceFunctionAtItsStart(0x1401D1CF0, CreateParkourActionAndPerformInitialTestIfFits_A_FullReplacement);
    // Several "Wallrun up" actions get created from this call.
    PresetScript_ReplaceFunctionAtItsStart(0x1401D1E50, CreateParkourActionAndPerformInitialTestIfFits_B_FullReplacement);
    PresetScript_ReplaceFunctionAtItsStart(0x140133B00, SortAndSelectBestMatchingAction_FullReplacement);
}
void ParkourDebuggingPatch::OnBeforeActivate()
{
    ImGui3D::CustomDraw::CustomDraw_Subscribe(g_ParkourVisualization);
}
void ParkourDebuggingPatch::OnBeforeDeactivate()
{
    ImGui3D::CustomDraw::CustomDraw_Unsubscribe(g_ParkourVisualization);
}

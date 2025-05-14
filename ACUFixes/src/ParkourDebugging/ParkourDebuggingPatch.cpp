#include "pch.h"

#include "ParkourDebugging/ParkourDebuggingPatch.h"

#include "vmath/vmath.h"

#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"
#include "ACU/SharedPtr.h"
#include "ACU/World.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU_DefineNativeFunction.h"

#include "MyLog.h"
#include "ImGui3D/ImGui3D.h"
#include "ImGui3D/ImGui3DCustomDraw.h"

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
class Entity;
class EntityGroup;
class ParkourAction_Commonbase : public AvailableParkourAction
{
public:
    // If climbing wall, this is where your hands were before the move started. If on beam, this is where your feet were.
    // If vaulting, this is where your feet start.
    Vector4f locationAnchorSrc; //0x0010
    char pad_0020[16]; //0x0020
    // If climbing wall, this is where your hands grab after finishing the move. If on beam, this is where your feet land.
    // If vaulting, this is where you grab to begin the vault, not where you land.
    Vector4f locationAnchorDest; //0x0030
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
    }
    ParkourVisualization() { ImGui3D::CustomDraw::CustomDraw_Subscribe(*this); }
    ~ParkourVisualization() { ImGui3D::CustomDraw::CustomDraw_Unsubscribe(*this); }
} g_ParkourVisualization;
void WhenReturningBestMatchingMove_LogIt(AllRegisters* params)
{
    int bestMatchMoveIdx = (int&)(params->GetRAX());
    if (bestMatchMoveIdx == -1)
    {
        return;
    }
    SmallArray<ParkourAction_Commonbase*>& actionsArray = *(SmallArray<ParkourAction_Commonbase*>*)params->rsi_;
    ParkourAction_Commonbase* bestMatchMove = actionsArray[bestMatchMoveIdx];
    EnumParkourAction actionType = bestMatchMove->GetEnumParkourAction();
    if (actionType == EnumParkourAction::wallrunUpFromGroundFailed_mb)
    {
        int x = 0;
    }
    LOG_DEBUG(ParkourLogger
        , "Selected: %7.2f %7.2f %7.2f (%d == %s)"
        , bestMatchMove->locationAnchorSrc.x
        , bestMatchMove->locationAnchorSrc.y
        , bestMatchMove->locationAnchorSrc.z
        , actionType
        , enum_reflection<EnumParkourAction>::GetString(actionType)
    );
    //ImGui3D::DrawLocationNamed((Vector3f&)bestMatchMove->locationAnchorSrc, "locationAnchorSrc");
    //ImGui3D::DrawLocationNamed((Vector3f&)bestMatchMove->locationAnchorDest, "locationAnchorDest");
    //ImGui3D::DrawLocationNamed((Vector3f&)bestMatchMove->handsLocationTo_right_mb, "handsLocationTo_right_mb");
    //ImGui3D::DrawLocationNamed((Vector3f&)bestMatchMove->handsLocationTo_left_mb, "handsLocationTo_left_mb");
    g_ParkourVisualization.m_History_SelectedMoves.Add(
        (Vector3f&)bestMatchMove->locationAnchorDest
        , World::GetSingleton()->clockInWorldWithSlowmotion.GetCurrentTimeFloat()
    );
}
std::vector<EnumParkourAction> g_UnidentifiedParkourActions = {
    EnumParkourAction::unk0,
    EnumParkourAction::stumbleAtEdge_mb,
    EnumParkourAction::leapBeamToVShape,
    EnumParkourAction::unk7,
    EnumParkourAction::passementFromBeam_A,
    EnumParkourAction::passementFromBeam_C,
    EnumParkourAction::passementD,
    EnumParkourAction::unk16,
    EnumParkourAction::unk17,
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
    auto FindIndexForCriterium = [&availableParkourActions](const std::wstring_view& critName, auto criterium) -> std::optional<int>
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

    auto foundIt = FindIndexForCriterium(L"IsRare", IsRare);
    if (!foundIt)
    {
        return {};
    }
    return *foundIt;
}
void WhenGatheredMoves_FilterFnPrologue_LogBeforeFiltered(AllRegisters* params)
{
    SmallArray<ParkourAction_Commonbase*>& moves = **(SmallArray<ParkourAction_Commonbase*>**)(params->GetRSP() + 0x48);
    if (moves.size == 0) return;
    LOG_DEBUG(ParkourLogger, "Num potential actions before filtering: %d"
        , moves.size
    );
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
DEFINE_GAME_FUNCTION(AvailableParkourAction__FinalFilter1, 0x1401D4DE0, uint64, __fastcall, (AvailableParkourAction* p_parkourAction, uint64 a2, uint64 a3, Entity* p_playerEntity));
void WhenPerformingFinalFilter1OnSortedMoves_ForceTurnInPalaisDeLuxembourgCorners(AllRegisters* params)
{
    AvailableParkourAction* parkourAction = (AvailableParkourAction*)params->rcx_;
    Entity* ent = (Entity*)params->r9_;
    uint64 result = AvailableParkourAction__FinalFilter1(parkourAction, params->rdx_, params->r8_, ent);
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
ParkourDebuggingPatch::ParkourDebuggingPatch()
{
    uintptr_t whenReturningBestMatchingMove = 0x140133D4B;
    PresetScript_CCodeInTheMiddle(whenReturningBestMatchingMove, 5,
        WhenReturningBestMatchingMove_LogIt, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenGatheredMoves_FilterFnPrologue = 0x140133B00;
    PresetScript_CCodeInTheMiddle(whenGatheredMoves_FilterFnPrologue, 7,
        WhenGatheredMoves_FilterFnPrologue_LogBeforeFiltered, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenPerformingFinalFilter1OnSortedMoves = 0x140133D16;
    PresetScript_CCodeInTheMiddle(whenPerformingFinalFilter1OnSortedMoves, 5,
        WhenPerformingFinalFilter1OnSortedMoves_ForceTurnInPalaisDeLuxembourgCorners, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
}
void ParkourDebuggingPatch::OnBeforeActivate()
{
    ImGui3D::CustomDraw::CustomDraw_Subscribe(g_ParkourVisualization);
}
void ParkourDebuggingPatch::OnBeforeDeactivate()
{
    ImGui3D::CustomDraw::CustomDraw_Unsubscribe(g_ParkourVisualization);
}

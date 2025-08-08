#include "pch.h"

#include "ParkourLog.h"

#include "ACU/World.h"
#include "AvailableParkourAction.h"
#include "ImGuiCTX.h"

#include "MyLog.h"

static DEFINE_LOGGER_CONSOLE_AND_FILE(ParkourLogger, "[Parkour]");








ParkourActionLogged::ParkourActionLogged(AvailableParkourAction& action, size_t indexInOrderOfCreationInCycle)
    : m_ActionPtr(&action)
    , m_FancyVTable((uint64)action.fancyVTable)
    , m_ActionType(action.GetEnumParkourAction())
    , m_LocationSrc((Vector3f&)action.locationAnchorSrc)
    , m_LocationDst((Vector3f&)action.locationAnchorDest)
    , m_DirSrc((Vector3f&)action.direction_20)
    , m_DirDstFacingOut((Vector3f&)action.directionDestFacingOut)
    , m_Index(indexInOrderOfCreationInCycle)
{}

void ParkourCycleLogged::LogActionInitialCreation(AvailableParkourAction& action, bool& isDiscarded_immediatelyAfterCreation)
{
    if (m_IsDisabled) return;
    ParkourActionLogged& recordForAction = MakeRecordForAction(action);
    auto& parkourLog = ParkourLog::GetSingleton();
    if (parkourLog.m_EnforcedMove)
    {
        if (action.GetEnumParkourAction() == parkourLog.m_EnforcedMove->m_ActionType)
        {
            isDiscarded_immediatelyAfterCreation = false;
        }
    }
    recordForAction.m_IsDiscarded_immediatelyAfterCreation = isDiscarded_immediatelyAfterCreation;
}
void ParkourCycleLogged::LogActionBeforeFiltering(AvailableParkourAction& action, float fitness, bool& isDiscarded_becauseFitnessWeightTooLow)
{
    if (m_IsDisabled) return;
    ParkourActionLogged& recordForAction = this->GetOrMakeRecordForAction(action);
    auto& parkourLog = ParkourLog::GetSingleton();
    if (parkourLog.m_EnforcedMove)
    {
        if (action.GetEnumParkourAction() == parkourLog.m_EnforcedMove->m_ActionType)
        {
            isDiscarded_becauseFitnessWeightTooLow = false;
        }
    }
    recordForAction.m_FitnessWeight = fitness;
    recordForAction.m_IsDiscarded_becauseFitnessWeightTooLow = isDiscarded_becauseFitnessWeightTooLow;
}
std::optional<float> GetModWeightForAction(AvailableParkourAction& action);
float ParkourCycleLogged::LogActionWeights(AvailableParkourAction& action, float defaultWeight, float totalWeight)
{
    if (m_IsDisabled) return totalWeight;
    ParkourActionLogged& recordForAction = this->GetOrMakeRecordForAction(action);
    std::optional<float> modWeight = GetModWeightForAction(action);
    recordForAction.m_UsedModWeight = modWeight;
    if (modWeight)
        totalWeight *= *modWeight;
    recordForAction.m_DefaultWeight = defaultWeight;
    recordForAction.m_TotalWeight = totalWeight;
    return totalWeight;
}
void LogActionsBeforeFiltering_ConsoleAnd3D(SmallArray<AvailableParkourAction*>& moves);
void LogActionBestMatch_ConsoleAnd3D(AvailableParkourAction& bestMatchMove);
void ParkourCycleLogged::LogActionsBeforeFiltering(SmallArray<AvailableParkourAction*>& allActionsBeforeFiltering)
{
    if (m_IsDisabled) return;
    LogActionsBeforeFiltering_ConsoleAnd3D(allActionsBeforeFiltering);
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
    LogActionBestMatch_ConsoleAnd3D(bestMatchMove);
}

void ParkourCycleLogged::LogAndChangeFinalSelection(AvailableParkourAction*& selectionAfterGameAndCallbacks, SmallArray<AvailableParkourAction*>& allActionsSorted)
{
    if (m_IsDisabled) return;
    auto& parkourLog = ParkourLog::GetSingleton();
    if (parkourLog.m_EnforcedMove)
    {
        AvailableParkourAction* bestAction = nullptr;
        float bestDistanceSqr = std::numeric_limits<float>::max();
        float radiusSqr = parkourLog.m_EnforcedMove->m_Radius * parkourLog.m_EnforcedMove->m_Radius;
        for (AvailableParkourAction* action : allActionsSorted)
        {
            if (action->GetEnumParkourAction() != parkourLog.m_EnforcedMove->m_ActionType) continue;
            const Vector3f& pos = (Vector3f&)action->locationAnchorDest;
            const float distToEnforcedCenterSqr = (pos - parkourLog.m_EnforcedMove->m_Position).lengthSq();
            if (distToEnforcedCenterSqr > radiusSqr) continue;
            const Vector3f& dir = (Vector3f&)action->directionDestFacingOut;
            const float minDirectionAlignment = 0.8f;
            if (parkourLog.m_EnforcedMove->m_DirectionFacingOut.dotProduct(dir) < minDirectionAlignment) continue;
            if (distToEnforcedCenterSqr > bestDistanceSqr) continue;

            bestDistanceSqr = distToEnforcedCenterSqr;
            bestAction = action;
        }
        if (bestAction)
        {
            ParkourActionLogged& record = this->GetOrMakeRecordForAction(*bestAction);
            selectionAfterGameAndCallbacks = bestAction;
            record.m_HasBeenEnforced = true;
        }
    }
}
ParkourLog::ParkourLog()
    : m_DisabledDummyCycle(new ParkourCycleLogged())
{
    m_RecentParkourCycles.reserve(1024);
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
void ParkourLog::DrawDisplayControls()
{
    ImGui::PushStyleCompact();
    ImGui::Checkbox("Discarded early (can sometimes impact performance)", &m_DisplaySettings.m_ShowDiscardedEarly);
    ImGui::Checkbox("Discarded late", &m_DisplaySettings.m_ShowDiscardedLate);
    ImGui::Checkbox("Nondiscarded", &m_DisplaySettings.m_ShowNondiscarded);
    ImGui::NewLine();
    ImGui::Checkbox("More than one recent cycle (can sometimes impact performance)", &m_DisplaySettings.m_ShowMoreThanOneOfTheRecentCycles);
    {
        ImGuiCTX::Indent _ind;
        ImGui::Checkbox("Skip fully discarded cycles", &m_DisplaySettings.m_ShowMoreThanOneOfTheRecentCycles_SkipAllDiscarded);
    }
    ImGui::PopStyleCompact();
}
bool ParkourLog::IsActionShouldBeDisplayed(ParkourActionLogged& action)
{
    if (action.m_IsDiscarded_immediatelyAfterCreation)
        return m_DisplaySettings.m_ShowDiscardedEarly;
    if (action.m_IsDiscarded_becauseFitnessWeightTooLow)
        return m_DisplaySettings.m_ShowDiscardedLate;
    if (!m_DisplaySettings.m_ShowNondiscarded)
        return false;
    return true;
}

extern bool g_IsParkourDebuggingActive;
std::shared_ptr<ParkourCycleLogged> ParkourLog::GetCurrentLoggedParkourCycle()
{
    const bool isParkourLogDisabled = !g_IsParkourDebuggingActive;
    if (isParkourLogDisabled) return m_DisabledDummyCycle;
    auto PushNewToHistory = [this](uint64 timestamp) -> std::shared_ptr<ParkourCycleLogged>& {
        size_t curSize = m_RecentParkourCycles.size();
        if (curSize >= m_MaxRetainNum)
        {
            m_RecentParkourCycles.erase(m_RecentParkourCycles.begin(), m_RecentParkourCycles.begin() + (curSize - m_MaxRetainNum + 1));
        }
        return m_RecentParkourCycles.emplace_back(std::make_shared<ParkourCycleLogged>(timestamp));
        };

    uint64 currentTimestamp = World::GetSingleton()->clockInWorldWithSlowmotion.currentTimestamp;
    std::lock_guard _lock{ m_Mutex };

    if (!m_RecentParkourCycles.size())
        return PushNewToHistory(currentTimestamp);
    auto& latest = m_RecentParkourCycles.back();
    if (latest->m_Timestamp != currentTimestamp)
        return PushNewToHistory(currentTimestamp);
    return latest;
}
std::shared_ptr<ParkourCycleLogged> ParkourLog::GetLatestLoggedParkourCycle()
{
    std::lock_guard _lock{ m_Mutex };
    if (m_RecentParkourCycles.size()) return m_RecentParkourCycles.back();
    return {};
}
std::vector<std::shared_ptr<ParkourCycleLogged>> ParkourLog::GetRecentCycles()
{
    std::lock_guard _lock{ m_Mutex };
    return m_RecentParkourCycles;
}
std::shared_ptr<ParkourCycleLogged> GetCurrentLoggedParkourCycle()
{
    return ParkourLog::GetSingleton().GetCurrentLoggedParkourCycle();
}

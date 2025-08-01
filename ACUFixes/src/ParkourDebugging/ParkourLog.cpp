#include "pch.h"

#include "ParkourLog.h"

#include "ACU/World.h"
#include "AvailableParkourAction.h"
#include "MyLog.h"

static DEFINE_LOGGER_CONSOLE_AND_FILE(ParkourLogger, "[Parkour]");








ParkourActionLogged::ParkourActionLogged(AvailableParkourAction& action, size_t indexInOrderOfCreationInCycle)
    : m_ActionPtr(&action)
    , m_ActionType(action.GetEnumParkourAction())
    , m_LocationSrc((Vector3f&)action.locationAnchorSrc)
    , m_LocationDst((Vector3f&)action.locationAnchorDest)
    , m_DirSrc((Vector3f&)action.direction_20)
    , m_DirDst((Vector3f&)action.direction_40)
    , m_Index(indexInOrderOfCreationInCycle)
{}

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

extern bool g_IsParkourDebuggingActive;
std::shared_ptr<ParkourCycleLogged> ParkourLog::GetCurrentLoggedParkourCycle()
{
    const bool isParkourLogDisabled = !g_IsParkourDebuggingActive;
    if (isParkourLogDisabled) return m_DisabledDummyCycle;

    uint64 currentTimestamp = World::GetSingleton()->clockInWorldWithSlowmotion.currentTimestamp;
    if (!m_LatestParkourCycle || m_LatestParkourCycle->m_Timestamp != currentTimestamp)
        m_LatestParkourCycle = std::make_shared<ParkourCycleLogged>(currentTimestamp);
    return m_LatestParkourCycle;
}
std::shared_ptr<ParkourCycleLogged> GetCurrentLoggedParkourCycle()
{
    return ParkourLog::GetSingleton().GetCurrentLoggedParkourCycle();
}

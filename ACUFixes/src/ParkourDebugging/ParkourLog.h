#pragma once

#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"
#include "vmath/vmath.h"
#include "EnumParkourAction.h"

class AvailableParkourAction;
class ParkourAction_Commonbase;

class ParkourActionLogged
{
public:
    // Used for identification within the log, will definitely become invalid after the "parkour cycle" finishes execution.
    // Note that this address probably isn't a good enough identifier on its own.
    // The actions can be discarded immediately after creation, so it's possible
    // that several different actions will be created at the same address within the same cycle.
    // Though I guess no more than one can be not-discarded-immediately-after-creation.
    AvailableParkourAction* m_ActionPtr;
    ParkourActionLogged(AvailableParkourAction& action, size_t indexInOrderOfCreationInCycle);
    size_t m_Index; // To keep track of the order of creation.

    EnumParkourAction m_ActionType;
    uint64 m_FancyVTable;
    Vector3f m_LocationSrc; // I think these are the same in every action of the parkour cycle.
    Vector3f m_DirSrc;      // I think these are the same in every action of the parkour cycle.
    Vector3f m_LocationDst;
    Vector3f m_DirDstFacingOut;
    bool m_IsDiscarded_immediatelyAfterCreation = false;
    std::optional<float> m_FitnessWeight;
    bool m_IsDiscarded_becauseFitnessWeightTooLow = false;
    std::optional<float> m_DefaultWeight;
    std::optional<float> m_TotalWeight;
    std::optional<bool> m_ResultOfFinalFilter1;
    std::optional<bool> m_ResultOfFinalFilter2;
    bool m_IsTheSelectedBestMatch = false;

    bool m_IsHighlightedInEditor = false;
    bool m_IsSelectedInEditor = false;
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
    void LogActionsBeforeFiltering(SmallArray<AvailableParkourAction*>& allActionsBeforeFiltering);
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

class ParkourLog
{
private:
    std::vector<std::shared_ptr<ParkourCycleLogged>> m_RecentParkourCycles;
    size_t m_MaxRetainNum = 1024;
    size_t m_MaxDisplayNum = 10;
    std::mutex m_Mutex;
public:
    std::shared_ptr<ParkourCycleLogged> GetCurrentLoggedParkourCycle();
    // Doesn't construct a new cycle.
    std::shared_ptr<ParkourCycleLogged> GetLatestLoggedParkourCycle();
    std::vector<std::shared_ptr<ParkourCycleLogged>> GetRecentCycles();

public:
    struct DisplaySettings
    {
        bool m_ShowDiscardedEarly = false;
        bool m_ShowDiscardedLate = true;
        bool m_ShowNondiscarded = true;
        bool m_ShowMoreThanOneOfTheRecentCycles = true;
    } m_DisplaySettings;
    void DrawDisplayControls();
    bool IsActionShouldBeDisplayed(ParkourActionLogged& action);

public:
    static ParkourLog& GetSingleton() { static ParkourLog singleton; return singleton; }
private:
    std::shared_ptr<ParkourCycleLogged> m_DisabledDummyCycle;
    ParkourLog();
};

// The returned shared_ptr is non-null.
std::shared_ptr<ParkourCycleLogged> GetCurrentLoggedParkourCycle();

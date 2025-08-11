#include "pch.h"

#include "GenericHooksInParkourFiltering.h"

#include "ACU/basic_types.h"
#include "ACU/Memory/ACUAllocs.h"
#include "ACU_DefineNativeFunction.h"
#include "EnumParkourAction.h"
#include "AvailableParkourAction.h"
#include "ParkourTester.h"

#include "ParkourDebugging/ParkourLog.h"

class Entity;

DEFINE_GAME_FUNCTION(AvailableParkourAction__FinalFilter1, 0x1401D4DE0, bool, __fastcall, (AvailableParkourAction* p_parkourAction, __m128* a2, uint64 a3, Entity* p_playerEntity));
DEFINE_GAME_FUNCTION(AvailableParkourAction__FinalFilter2, 0x1401D3000, bool, __fastcall, (AvailableParkourAction* p_parkourAction, Entity* p_playerEntity, __int64 a3));
DEFINE_GAME_FUNCTION(ConstructParkourAction_A, 0x1401CC990, AvailableParkourAction*, __fastcall, (EnumParkourAction p_actionEnum, __m128* a2, __m128* a3, __m128* p_movementVecWorld_mb, int a5, char a6, __int64 a7, AvailableParkourAction* p_previousAction_mb));
DEFINE_GAME_FUNCTION(ConstructParkourAction_B, 0x1401D1FC0, AvailableParkourAction*, __fastcall, (EnumParkourAction p_actionEnum, __m128* a2, __m128* a3, __m128* p_movementVecWorld_mb, int a5, char a6, __int64 a7, __int64 a8, Entity* p_player, AvailableParkourAction* p_previousAction_mb));
DEFINE_GAME_FUNCTION(AvailableParkourAction__InitializePlayerRef, 0x14015A570, void, __fastcall, (PlayerRefInParkourAction* p_playerRefOut, Entity* a2));
// Used for a SmallArray of plain types, where constructors/destructors don't matter.
DEFINE_GAME_FUNCTION(SmallArray_POD__RemoveGeneric, 0x142725F00, void, __fastcall, (void* smallArray, int p_idx, unsigned int p_elemSize));


#include "MyLog.h"
class CollisionProbeForParkour_mb
{
public:
    Vector4f locationAnchorDest; //0x0000
    Vector4f direction; //0x0010
    char pad_0020[20]; //0x0020
    float flt_34; //0x0034
    uint32 collisionLayer_mb; //0x0038
    char pad_003C[4]; //0x003C
    Vector4f pos40; //0x0040
    Vector4f pos50; //0x0050
    PlayerRefInParkourAction shared_buildingEntity; //0x0060
    char pad_0078[8]; //0x0078
}; //Size: 0x0080
assert_offsetof(CollisionProbeForParkour_mb, collisionLayer_mb, 0x38);
assert_offsetof(CollisionProbeForParkour_mb, shared_buildingEntity, 0x60);
assert_sizeof(CollisionProbeForParkour_mb, 0x80); // Allocated and constructed at 140184AEF
static DEFINE_LOGGER_CONSOLE_AND_FILE(ParkourLogger, "[Parkour]");
bool CreateParkourActionAndPerformInitialTestIfFits_A_FullReplacement(
    EnumParkourAction actionType,
    __m128* p_locationOfOrigin,
    __m128* a3,
    __m128* p_directionOfMovementInputWorldSpace,
    float a5, int a6, char a7, CollisionProbeForParkour_mb* a8,
    Entity* p_player,
    AvailableParkourAction* p_previousAction_mb,
    AvailableParkourAction** const p_newAction_out,
    float a12,
    float p_epsilon_mb)
{
    AvailableParkourAction* newAction = ConstructParkourAction_A(actionType, p_locationOfOrigin, a3, p_directionOfMovementInputWorldSpace, a6, a7, (uint64)a8, p_previousAction_mb);
    *p_newAction_out = newAction;
    if (!newAction) return false;
    if (p_player)
        AvailableParkourAction__InitializePlayerRef(&newAction->playerRef, p_player);
    GET_AND_CAST_FANCY_FUNC(*newAction, ParkourActionKnownFancyVFuncs::Set2FloatsAfterCreation)(newAction, a12, p_epsilon_mb);
    bool isActionFits = GET_AND_CAST_FANCY_FUNC(*newAction, ParkourActionKnownFancyVFuncs::InitialTestIfActionFits)(
        newAction,
        p_locationOfOrigin,
        a3,
        p_directionOfMovementInputWorldSpace,
        a5,
        a6,
        (uint64)a8,
        p_player,
        p_previousAction_mb);
    std::shared_ptr<ParkourCycleLogged> currentCycle = GetCurrentLoggedParkourCycle();
    bool isDiscarded = !isActionFits;
    currentCycle->LogActionInitialCreation(*newAction, isDiscarded);
    currentCycle->m_LocationOfOrigin = *(Vector3f*)p_locationOfOrigin;
    currentCycle->m_DirectionOfMovementInputWorldSpace = *(Vector3f*)p_directionOfMovementInputWorldSpace;
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
    __m128* p_locationOfOrigin,
    __m128* a3,
    __m128* p_directionOfMovementInputWorldSpace,
    float a5, int a6, char a7, uint64 a8, uint64 a9,
    Entity* p_player,
    AvailableParkourAction* p_previousAction_mb,
    AvailableParkourAction** const p_newAction_out,
    float a13,
    float p_epsilon_mb)
{
    AvailableParkourAction* newAction = ConstructParkourAction_B(actionType, p_locationOfOrigin, a3, p_directionOfMovementInputWorldSpace, a6, a7, a8, a9, p_player, p_previousAction_mb);
    *p_newAction_out = newAction;
    if (!newAction) return false;
    if (p_player)
        AvailableParkourAction__InitializePlayerRef(&newAction->playerRef, p_player);
    GET_AND_CAST_FANCY_FUNC(*newAction, ParkourActionKnownFancyVFuncs::Set2FloatsAfterCreation)(newAction, a13, p_epsilon_mb);
    bool isActionFits = GET_AND_CAST_FANCY_FUNC(*newAction, ParkourActionKnownFancyVFuncs::InitialTestIfActionFits)(
        newAction,
        p_locationOfOrigin,
        a3,
        p_directionOfMovementInputWorldSpace,
        a5,
        a6,
        a9,
        p_player,
        p_previousAction_mb);
    std::shared_ptr<ParkourCycleLogged> currentCycle = GetCurrentLoggedParkourCycle();
    bool isDiscarded = !isActionFits;
    currentCycle->LogActionInitialCreation(*newAction, isDiscarded);
    currentCycle->m_LocationOfOrigin = *(Vector3f*)p_locationOfOrigin;
    currentCycle->m_DirectionOfMovementInputWorldSpace = *(Vector3f*)p_directionOfMovementInputWorldSpace;
    if (isDiscarded)
    {
        newAction->Unk008_Destroy(0);
        ACU::Memory::ACUDeallocateBytes((byte*)newAction);
        *p_newAction_out = nullptr;
        return false;
    }
    return true;
}
static std::optional<int> FindIndexForAction(AvailableParkourAction& action, SmallArray<AvailableParkourAction*>& p_parkourSensorsResults)
{
    auto foundIt = std::find(p_parkourSensorsResults.begin(), p_parkourSensorsResults.end(), &action);
    if (foundIt != p_parkourSensorsResults.end())
    {
        return foundIt - p_parkourSensorsResults.begin();
    }
    return {};
}
constexpr int RESULT_OF_PARKOUR_SORT_AND_SELECT__NO_ACTIONS_ACCEPTED = -1;
int SortAndSelectBestMatchingAction_FullReplacement_inner(
    ParkourTester* parkourTester
    , __m128* p_locationOfOrigin
    , uint64 a3
    , __m128* p_directionOfMovementInputWorldSpace
    , float a5
    , int a6
    , char a7
    , __int64 a8
    , SmallArray<AvailableParkourAction*>& p_parkourSensorsResults
    , std::shared_ptr<ParkourCycleLogged>& currentCycle
)
{
    currentCycle->LogActionsBeforeFiltering(p_parkourSensorsResults);

    auto& allParkourCallbacks = GenericHooksInParkourFiltering::GetSingleton()->m_Callbacks;
    for (ParkourCallbacks* parkourCallbacks : allParkourCallbacks)
    {
        if (!parkourCallbacks->ChooseBeforeFiltering_fnp) continue;
        AvailableParkourAction* selectedBeforeFiltering = parkourCallbacks->ChooseBeforeFiltering_fnp(parkourCallbacks->m_UserData, p_parkourSensorsResults);
        if (selectedBeforeFiltering)
        {
            if (std::optional<int> idx = FindIndexForAction(*selectedBeforeFiltering, p_parkourSensorsResults))
            {
                return *idx;
            }
        }
    }
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
            weights1st.totalWeight = currentCycle->LogActionWeights(*actionFirst, weights1st.defaultWeight, weights1st.totalWeight);
            weights2nd.totalWeight = currentCycle->LogActionWeights(*action2nd, weights2nd.defaultWeight, weights2nd.totalWeight);
            return weights1st.totalWeight > weights2nd.totalWeight;
            };
        std::sort(p_parkourSensorsResults.begin(), p_parkourSensorsResults.end(), SortByDescendingTotalFitness);
    }
    std::optional<int> selectedBestMatch;
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
        break;
    }

    for (ParkourCallbacks* parkourCallbacks : allParkourCallbacks)
    {
        if (!parkourCallbacks->ChooseAfterSorting_fnp) continue;
        AvailableParkourAction* selectedAfterSorting =
            parkourCallbacks->ChooseAfterSorting_fnp(parkourCallbacks->m_UserData, p_parkourSensorsResults, selectedBestMatch ? p_parkourSensorsResults[*selectedBestMatch] : nullptr);
        if (selectedAfterSorting)
        {
            if (std::optional<int> idx = FindIndexForAction(*selectedAfterSorting, p_parkourSensorsResults))
            {
                selectedBestMatch = idx;
                break;
            }
        }
    }

    int result = selectedBestMatch ? *selectedBestMatch : RESULT_OF_PARKOUR_SORT_AND_SELECT__NO_ACTIONS_ACCEPTED;

    return result;
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
    SmallArray<AvailableParkourAction*>& p_parkourSensorsResults)
{
    std::shared_ptr<ParkourCycleLogged> currentCycle = GetCurrentLoggedParkourCycle();
    int selectedIdx = SortAndSelectBestMatchingAction_FullReplacement_inner(
        parkourTester,
        p_locationOfOrigin,
        a3, p_directionOfMovementInputWorldSpace,
        a5, a6, a7, a8,
        p_parkourSensorsResults,
        currentCycle
    );
    if (selectedIdx != RESULT_OF_PARKOUR_SORT_AND_SELECT__NO_ACTIONS_ACCEPTED)
    {
        AvailableParkourAction* selectedAction = p_parkourSensorsResults[selectedIdx];
        currentCycle->LogActionWhenReturningBestMatch(*selectedAction);
    }
    return selectedIdx;
}
GPH_Creation::GPH_Creation()
{
    // Most "action types" are created from this call.
    PresetScript_ReplaceFunctionAtItsStart(0x1401D1CF0, CreateParkourActionAndPerformInitialTestIfFits_A_FullReplacement);
    // Several "Wallrun up" actions get created from this call.
    PresetScript_ReplaceFunctionAtItsStart(0x1401D1E50, CreateParkourActionAndPerformInitialTestIfFits_B_FullReplacement);
}
GPH_SortAndSelect::GPH_SortAndSelect()
{
    PresetScript_ReplaceFunctionAtItsStart(0x140133B00, SortAndSelectBestMatchingAction_FullReplacement);
}
SharedHookActivator::SharedHookActivator(std::function<void()> onCreate, std::function<void()> onDestroy)
    : m_OnCreate(onCreate)
    , m_OnDestroy(onDestroy)
{
    m_OnCreate();
}


SharedHookActivator::~SharedHookActivator()
{
    m_OnDestroy();
}
std::shared_ptr<SharedHookActivator> GenericHooksInParkourFiltering::RequestGPHCreation()
{
    std::lock_guard _lock{ m_Mutex };
    std::shared_ptr<SharedHookActivator> activator = m_Activator_GPHCreation.lock();
    if (activator) return activator;
    activator = std::make_shared<SharedHookActivator>(
        [&]() { gph_creation.Activate(); },
        [&]() { gph_creation.Deactivate(); }
    );
    m_Activator_GPHCreation = activator;
    return activator;
}
std::shared_ptr<SharedHookActivator> GenericHooksInParkourFiltering::RequestGPHSortAndSelect()
{
    std::lock_guard _lock{ m_Mutex };
    std::shared_ptr<SharedHookActivator> activator = m_Activator_GPHSortAndSelect.lock();
    if (activator) return activator;
    activator = std::make_shared<SharedHookActivator>(
        [&]() { gph_sortAndSelect.Activate(); },
        [&]() { gph_sortAndSelect.Deactivate(); }
    );
    m_Activator_GPHSortAndSelect = activator;
    return activator;
}

// This singleton is shared_ptr, inconsistent with the rest of the singletons I use
// because I ran into the "deinitialization order fiasco".
// If I just use "static GenericHooksInParkourFiltering" in the GetSingleton()
// then the order of calling of constructors is as follows:
//      std::optional g_MyHacks;
//      ...
//      g_MyHacks.emplace()
//          ParkourActionsExtraProcessing::ctor()
//              GenericHooksInParkourFiltering::GetSingleton() { static GenericHooksInParkourFiltering singleton; return singleton; }
// which means that the order of completed construction is rearranged:
//      std::optional g_MyHacks
//      ...
//      static GenericHooksInParkourFiltering singleton
//      ParkourActionsExtraProcessing
// And so the order of pushed "atexit"s is:
//      std::optional g_MyHacks
//      ...
//      static GenericHooksInParkourFiltering singleton
// And so the order of destructors is:
//      static GenericHooksInParkourFiltering singleton::dtor()
//          m_Callbacks::dtor()
//      ...
//      std::optional g_MyHacks::dtor()
//          AutoAssemblerWrapper<ParkourActionsExtraProcessing>::dtor()
//              AutoAssemblerWrapper<ParkourActionsExtraProcessing>::OnBeforeDeactivate()
//                  ^^^---  And this is where GenericHooksInParkourFiltering::Unsubscribe() is called,
//                          when the GenericHooksInParkourFiltering::singleton::m_Callbacks is already
//                          destroyed, and its erase() throws an "vector erase iterator outside range" assertion and crashes.
// So because of std::optional's delayed construction, the static GenericHooksInParkourFiltering
// is guaranteed to be constructed later than g_MyHacks,
// and automatic destruction order is guaranteed to be incorrect.
// Basically, I need to keep the instance of GenericHooksInParkourFiltering alive for longer
// than the global g_MyHacks
// and the way I choose to do it here is shared_ptr.
// It's not great, not foolproof, but it solves this crash.
// I guess something better would be a dedicated initialization stage for the shared patches/features?
std::shared_ptr<GenericHooksInParkourFiltering>& GenericHooksInParkourFiltering::GetSingleton()
{
    static auto singleton = std::make_shared<GenericHooksInParkourFiltering>();
    return singleton;
}
void GenericHooksInParkourFiltering::Subscribe(ParkourCallbacks& callbacks)
{
    auto insertIt = m_Callbacks.begin();
    for (; insertIt != m_Callbacks.end(); insertIt++)
    {
        if (callbacks.m_CallbackPriority < (*insertIt)->m_CallbackPriority) break;
    }
    m_Callbacks.insert(insertIt, &callbacks);
}
void GenericHooksInParkourFiltering::Unsubscribe(ParkourCallbacks& callbacks)
{
    std::erase(m_Callbacks, &callbacks);
}

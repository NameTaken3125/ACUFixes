#pragma once

#include "ACU/SmallArray.h"

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct GPH_Creation : AutoAssemblerCodeHolder_Base
{
    GPH_Creation();
};
struct GPH_SortAndSelect : AutoAssemblerCodeHolder_Base
{
    GPH_SortAndSelect();
};

// Used as basically a refcount for code hooks.
// A particular hook will be installed/Activated while an associated instance
// of activator is alive.
struct SharedHookActivator
{
    std::function<void()> m_OnCreate;
    std::function<void()> m_OnDestroy;

    SharedHookActivator(std::function<void()> onCreate, std::function<void()> onDestroy);
    ~SharedHookActivator();

    SharedHookActivator(const SharedHookActivator& rhs) = delete;
    SharedHookActivator& operator=(const SharedHookActivator& rhs) = delete;
};

class AvailableParkourAction;
struct ParkourCallbacks
{
    AvailableParkourAction* (*ChooseBeforeFiltering_fnp)(void* userData, SmallArray<AvailableParkourAction*>& actions) = nullptr;
    AvailableParkourAction* (*ChooseAfterSorting_fnp)(void* userData, SmallArray<AvailableParkourAction*>& actions, AvailableParkourAction* selectedByGame) = nullptr;
    void* m_UserData = nullptr;
    float m_CallbackPriority;
    const char* m_Name = nullptr;
};
class GenericHooksInParkourFiltering
{
public:
    AutoAssembleWrapper<GPH_Creation> gph_creation;
    AutoAssembleWrapper<GPH_SortAndSelect> gph_sortAndSelect;

    static std::shared_ptr<GenericHooksInParkourFiltering>& GetSingleton();
    std::shared_ptr<SharedHookActivator> RequestGPHCreation();
    std::shared_ptr<SharedHookActivator> RequestGPHSortAndSelect();

    void Subscribe(ParkourCallbacks& callbacks);
    void Unsubscribe(ParkourCallbacks& callbacks);
    std::vector<ParkourCallbacks*> m_Callbacks;

private:
    std::weak_ptr<SharedHookActivator> m_Activator_GPHCreation;
    std::weak_ptr<SharedHookActivator> m_Activator_GPHSortAndSelect;
    std::mutex m_Mutex;
};

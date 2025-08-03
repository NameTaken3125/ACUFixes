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
class ParkourCallbacks
{
public:
    virtual ~ParkourCallbacks() {}
    virtual AvailableParkourAction* ChooseBeforeFiltering(SmallArray<AvailableParkourAction*>& actions) { return nullptr; }
    virtual AvailableParkourAction* ChooseAfterSorting(SmallArray<AvailableParkourAction*>& actions, AvailableParkourAction* selectedByGame) { return nullptr; }
};
class GenericHooksInParkourFiltering
{
public:
    AutoAssembleWrapper<GPH_Creation> gph_creation;
    AutoAssembleWrapper<GPH_SortAndSelect> gph_sortAndSelect;

    static GenericHooksInParkourFiltering& GetSingleton() { static GenericHooksInParkourFiltering singleton; return singleton; }
    std::shared_ptr<SharedHookActivator> RequestGPHCreation();
    std::shared_ptr<SharedHookActivator> RequestGPHSortAndSelect();

    ParkourCallbacks* m_Callbacks = nullptr;

private:
    std::weak_ptr<SharedHookActivator> m_Activator_GPHCreation;
    std::weak_ptr<SharedHookActivator> m_Activator_GPHSortAndSelect;
    std::mutex m_Mutex;
};

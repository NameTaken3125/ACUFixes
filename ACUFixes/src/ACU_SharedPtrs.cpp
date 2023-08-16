#include "pch.h"

#include "ACU_SharedPtrs.h"
#include "ACU_DefineNativeFunction.h"

class AllManagedObjects_mb
{
public:

    // @helper_functions
    static AllManagedObjects_mb& GetSingleton() { return *(AllManagedObjects_mb*)0x14525BB70; }
};
DEFINE_GAME_FUNCTION(ManagedObjectsList__GetByHandle_mb, 0x1426EAF00, SharedPtrNew<ManagedObject>*, __fastcall, (AllManagedObjects_mb* objList, __int64 p_managedObjHandle_mb_opt, ManagedObject* a3));
DEFINE_GAME_FUNCTION(ManagedObjectsList__FreeObject_mb, 0x1426EC8E0, void, __fastcall, (AllManagedObjects_mb* objList, SharedPtrNew<ManagedObject>* a2));

void FreeSharedBlock_impl(SharedPtrNew<ManagedObject>& sharedBlockForManagedObject)
{
    ManagedObjectsList__FreeObject_mb(&AllManagedObjects_mb::GetSingleton(), &sharedBlockForManagedObject);
}

SharedPtrNew<ManagedObject>& FindOrMakeSharedBlockByHandleAndIncrementWeakrefcount(uint64 handle)
{
    SharedPtrNew<ManagedObject>* foundShared = nullptr;
    foundShared = ManagedObjectsList__GetByHandle_mb(&AllManagedObjects_mb::GetSingleton(), handle, nullptr);
    return *foundShared;
}

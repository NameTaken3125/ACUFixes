#include "pch.h"

#include "ACU/AllManagedObjects.h"
#include "ACU_SharedPtrs.h"
#include "ACU_DefineNativeFunction.h"

DEFINE_GAME_FUNCTION(AllManagedObjects__FindOrMakeSharedBlockByHandle_WeakRef, 0x1426EAF00, SharedPtrNew<ManagedObject>*, __fastcall, (AllManagedObjects_mb* objList, __int64 p_managedObjHandle_mb_opt, ManagedObject* a3));
DEFINE_GAME_FUNCTION(AllManagedObjects__FindOrMakeSharedBlockByHandle_StrongRef, 0x1426EB640, SharedPtrNew<ManagedObject>*, __fastcall, (AllManagedObjects_mb* objList, __int64 p_managedObjHandle_mb_opt, ManagedObject* a3));
DEFINE_GAME_FUNCTION(AllManagedObjects__FreeSharedBlock_mb, 0x1426EC8E0, void, __fastcall, (AllManagedObjects_mb* objList, SharedPtrNew<ManagedObject>* a2));

void FreeSharedBlock_impl(SharedPtrNew<ManagedObject>& sharedBlockForManagedObject)
{
    AllManagedObjects__FreeSharedBlock_mb(&AllManagedObjects_mb::GetSingleton(), &sharedBlockForManagedObject);
}

SharedPtrNew<ManagedObject>& FindOrMakeSharedBlockByHandleAndIncrementWeakrefcount(uint64 handle)
{
    SharedPtrNew<ManagedObject>* foundShared = nullptr;
    foundShared = AllManagedObjects__FindOrMakeSharedBlockByHandle_WeakRef(&AllManagedObjects_mb::GetSingleton(), handle, nullptr);
    return *foundShared;
}
SharedPtrNew<ManagedObject>& FindOrMakeSharedBlockByHandleAndIncrementStrongRefcount(uint64 handle)
{
    SharedPtrNew<ManagedObject>* foundShared = nullptr;
    foundShared = AllManagedObjects__FindOrMakeSharedBlockByHandle_WeakRef(&AllManagedObjects_mb::GetSingleton(), handle, nullptr);
    return *foundShared;
}
DEFINE_GAME_FUNCTION(AllManagedObjects__MarkObjectToBeFreedAfterZeroStrongRefcount, 0x142701270, void, __fastcall, (AllManagedObjects_mb* objList, SharedPtrNew<ManagedObject>* a2));
void MarkManagedObjectForFreeingAfterZeroStrongRefcount_impl(SharedPtrNew<ManagedObject>& sharedBlock)
{
    AllManagedObjects__MarkObjectToBeFreedAfterZeroStrongRefcount(&AllManagedObjects_mb::GetSingleton(), &sharedBlock);
}

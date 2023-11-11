#include "pch.h"

#include "ACU/AllManagedObjects.h"
#include "ACU_SharedPtrs.h"
#include "ACU_DefineNativeFunction.h"

DEFINE_GAME_FUNCTION(AllManagedObjects__FindOrMakeSharedBlockByHandle_WeakRef, 0x1426EAF00, SharedBlock*, __fastcall, (AllManagedObjects_mb* objList, __int64 p_managedObjHandle_mb_opt, ManagedObject* a3));
DEFINE_GAME_FUNCTION(AllManagedObjects__FindOrMakeSharedBlockByHandle_StrongRef, 0x1426EB640, SharedBlock*, __fastcall, (AllManagedObjects_mb* objList, __int64 p_managedObjHandle_mb_opt, ManagedObject* a3));
DEFINE_GAME_FUNCTION(AllManagedObjects__FreeSharedBlock_mb, 0x1426EC8E0, void, __fastcall, (AllManagedObjects_mb* objList, SharedBlock* a2));
DEFINE_GAME_FUNCTION(AllManagedObjects__MarkObjectToBeFreedAfterZeroStrongRefcount, 0x142701270, void, __fastcall, (AllManagedObjects_mb* objList, SharedBlock* a2));
void MarkManagedObjectForFreeingAfterZeroStrongRefcount_impl(SharedBlock& sharedBlock)
{
    AllManagedObjects__MarkObjectToBeFreedAfterZeroStrongRefcount(&AllManagedObjects_mb::GetSingleton(), &sharedBlock);
}
void FreeSharedBlock_impl(SharedBlock& sharedBlockForManagedObject)
{
    AllManagedObjects__FreeSharedBlock_mb(&AllManagedObjects_mb::GetSingleton(), &sharedBlockForManagedObject);
}

SharedBlock& FindOrMakeSharedBlockByHandleAndIncrementWeakrefcount(uint64 handle)
{
    SharedBlock* foundShared = nullptr;
    foundShared = AllManagedObjects__FindOrMakeSharedBlockByHandle_WeakRef(&AllManagedObjects_mb::GetSingleton(), handle, nullptr);
    return *foundShared;
}
SharedBlock& FindOrMakeSharedBlockByHandleAndIncrementStrongRefcount(uint64 handle)
{
    SharedBlock* foundShared = nullptr;
    foundShared = AllManagedObjects__FindOrMakeSharedBlockByHandle_StrongRef(&AllManagedObjects_mb::GetSingleton(), handle, nullptr);
    return *foundShared;
}

SharedBlock& FindSharedBlockByObjectAndIncrementWeakrefcount(ManagedObject& manObj)
{
    return *AllManagedObjects__FindOrMakeSharedBlockByHandle_WeakRef(&AllManagedObjects_mb::GetSingleton(), 0, &manObj);
}
SharedBlock& FindSharedBlockByObjectAndIncrementStrongRefcount(ManagedObject& manObj)
{
    return *AllManagedObjects__FindOrMakeSharedBlockByHandle_StrongRef(&AllManagedObjects_mb::GetSingleton(), 0, &manObj);
}

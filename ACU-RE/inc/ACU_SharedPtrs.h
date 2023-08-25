#pragma once

#include "ACU/ManagedObject.h"
#include "ACU/SharedPtr.h"


SharedPtrNew<ManagedObject>& FindOrMakeSharedBlockByHandleAndIncrementWeakrefcount(uint64 handle);
SharedPtrNew<ManagedObject>& FindOrMakeSharedBlockByHandleAndIncrementStrongRefcount(uint64 handle);

void FreeSharedBlock_impl(SharedPtrNew<ManagedObject>& sharedBlockForManagedObject);
void MarkManagedObjectForFreeingAfterZeroStrongRefcount_impl(SharedPtrNew<ManagedObject>& sharedBlock);
template<class ManagedObjectSubcls>
void DecrementWeakRefcount(SharedPtrNew<ManagedObjectSubcls>& sharedPtr)
{
    if (!_InterlockedDecrement(&sharedPtr.weakRefCount))
    {
        // Refcount (weak refcount maybe?) reached 0.
        if (!(sharedPtr.strongRefCountAndFlags & 0xFFFFFF))
        {
            SharedPtrNew<ManagedObject>* asManagedObjPtr = (SharedPtrNew<ManagedObject>*)&sharedPtr;
            FreeSharedBlock_impl(*asManagedObjPtr);
        }
    }
}
template<class ManagedObjectSubcls>
void DecrementStrongRefcount(SharedPtrNew<ManagedObjectSubcls>& sharedBlock)
{
    if (!(_InterlockedDecrement((volatile signed long*)&sharedBlock.strongRefCountAndFlags) & 0xFFFFFF))
    {
        SharedPtrNew<ManagedObject>* asManagedObjPtr = (SharedPtrNew<ManagedObject>*)&sharedBlock;
        MarkManagedObjectForFreeingAfterZeroStrongRefcount_impl(*asManagedObjPtr);
    }
}

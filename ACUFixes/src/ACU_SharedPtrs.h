#pragma once

#include "ACU/ManagedObject.h"
#include "ACU/SharedPtr.h"


SharedPtrNew<ManagedObject>& FindOrMakeSharedBlockByHandleAndIncrementWeakrefcount(uint64 handle);

void FreeSharedBlock_impl(SharedPtrNew<ManagedObject>& sharedBlockForManagedObject);
template<class ManagedObjectSubcls>
void DecrementWeakRefcount(SharedPtrNew<ManagedObjectSubcls>& sharedPtr)
{
    if (!_InterlockedDecrement(&sharedPtr.refCount))
    {
        // Refcount (weak refcount maybe?) reached 0.
        if (!(sharedPtr.dword_C & 0xFFFFFF))
        {
            SharedPtrNew<ManagedObject>* asManagedObjPtr = (SharedPtrNew<ManagedObject>*)&sharedPtr;
            FreeSharedBlock_impl(*asManagedObjPtr);
        }
    }
}

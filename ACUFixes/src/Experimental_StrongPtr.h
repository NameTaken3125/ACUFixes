#pragma once

#include <functional>

#include "ACU/basic_types.h"
#include "ACU_SharedPtrs.h"

SharedBlock* const g_emptySharedBlock = (SharedBlock*)0x14525BB58;
template<class ManagedObjectSubcls>
class ACUSharedPtr_Strong
{
public:
    // "Empty" shared ptr.
    ACUSharedPtr_Strong();
    ACUSharedPtr_Strong(uint64 managedObjectHandle);
    ACUSharedPtr_Strong(SharedPtrNew<ManagedObjectSubcls>& sharedBlock);
    ACUSharedPtr_Strong(const ACUSharedPtr_Strong& rhs);
    ACUSharedPtr_Strong& operator=(const ACUSharedPtr_Strong& rhs);
    ~ACUSharedPtr_Strong();

public:
    ManagedObjectSubcls* GetPtr() { return static_cast<ManagedObjectSubcls*>(m_SharedBlock.get().GetPtr()); }
    SharedPtrNew<ManagedObjectSubcls>& GetSharedBlock() { return (SharedPtrNew<ManagedObjectSubcls>&)m_SharedBlock.get(); }
    SharedPtrNew<ManagedObjectSubcls>& Release() { auto& freeSharedBlock = GetSharedBlock(); m_SharedBlock = *g_emptySharedBlock; return freeSharedBlock; }
    void Reset();
private:
    // In the game, the pointer to a shared block is never a `nullptr`.
    // When a shared ptr is reset to "empty", it just points to `g_emptySharedBlock` (== 0x14525BB58).
    // It's an optimisation thing, I guess, to avoid nullptr-checking when increasing/decreasing the refcount.
    std::reference_wrapper<SharedBlock> m_SharedBlock;
};
assert_sizeof(ACUSharedPtr_Strong<ManagedObject>, 8);


template<class ManagedObjectSubcls>
inline void ACUSharedPtr_Strong<ManagedObjectSubcls>::Reset()
{
    SharedBlock& sharedBlock = m_SharedBlock.get();
    m_SharedBlock = *g_emptySharedBlock;
    sharedBlock.DecrementStrongRefcount();
}
template<class ManagedObjectSubcls>
inline ACUSharedPtr_Strong<ManagedObjectSubcls>::ACUSharedPtr_Strong(SharedPtrNew<ManagedObjectSubcls>& sharedBlock)
    : m_SharedBlock(static_cast<SharedBlock&>(sharedBlock))
{
    sharedBlock.IncrementStrongRefcount();
}
template<class ManagedObjectSubcls>
inline ACUSharedPtr_Strong<ManagedObjectSubcls>::ACUSharedPtr_Strong(const ACUSharedPtr_Strong& rhs)
    : m_SharedBlock(rhs.m_SharedBlock)
{
    m_SharedBlock.get().IncrementStrongRefcount();
}
template<class ManagedObjectSubcls>
inline ACUSharedPtr_Strong<ManagedObjectSubcls>& ACUSharedPtr_Strong<ManagedObjectSubcls>::operator=(const ACUSharedPtr_Strong& rhs)
{
    SharedBlock& prevSharedBlock = m_SharedBlock.get();
    m_SharedBlock = rhs.m_SharedBlock;
    m_SharedBlock.get().IncrementStrongRefcount();
    prevSharedBlock.DecrementStrongRefcount();
    return *this;
}
template<class ManagedObjectSubcls>
inline ACUSharedPtr_Strong<ManagedObjectSubcls>::ACUSharedPtr_Strong(uint64 handle)
    : m_SharedBlock(FindOrMakeSharedBlockByHandleAndIncrementStrongRefcount(handle))
{}
template<class ManagedObjectSubcls>
inline ACUSharedPtr_Strong<ManagedObjectSubcls>::ACUSharedPtr_Strong()
    : m_SharedBlock(*g_emptySharedBlock)
{}
template<class ManagedObjectSubcls>
inline ACUSharedPtr_Strong<ManagedObjectSubcls>::~ACUSharedPtr_Strong()
{
    m_SharedBlock.get().DecrementStrongRefcount();
}

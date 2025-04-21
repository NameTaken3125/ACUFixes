#pragma once

#include <functional>

#include "ACU/basic_types.h"
#include "ACU_SharedPtrs.h"

SharedBlock* const g_emptySharedBlock = (SharedBlock*)0x14525BB58;

namespace ACU {
template<class ManagedObjectSubcls> class WeakPtr;
}

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
    ManagedObjectSubcls* GetPtr() const { return static_cast<ManagedObjectSubcls*>(m_SharedBlock.get().GetPtr()); }
    uint64 GetHandle() const { return m_SharedBlock.get().handle; }
    SharedPtrNew<ManagedObjectSubcls>& GetSharedBlock() { return (SharedPtrNew<ManagedObjectSubcls>&)m_SharedBlock.get(); }
    // Resets to empty and _does_not_ decrement refcount.
    SharedPtrNew<ManagedObjectSubcls>& Release() { auto& freeSharedBlock = GetSharedBlock(); m_SharedBlock = *g_emptySharedBlock; return freeSharedBlock; }
    // Resets to empty and decrements refcount.
    void Reset();
    // True means that it it's set to empty block, not that the refcount is 0.
    // To check whether the managed object is valid, just use GetPtr().
    bool IsEmpty() const { return &m_SharedBlock.get() == g_emptySharedBlock; }
private:
    // In the game, the pointer to a shared block is never a `nullptr`.
    // When a shared ptr is reset to "empty", it just points to `g_emptySharedBlock` (== 0x14525BB58).
    // It's an optimisation thing, I guess, to avoid nullptr-checking when increasing/decreasing the refcount.
    std::reference_wrapper<SharedBlock> m_SharedBlock;
    friend ACU::WeakPtr<ManagedObjectSubcls>;
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

namespace ACU
{
template<class ManagedObjectSubcls>
class WeakPtr
{
public:
    // "Empty" shared ptr.
    WeakPtr();
    WeakPtr(uint64 managedObjectHandle);
    WeakPtr(SharedPtrNew<ManagedObjectSubcls>& sharedBlock);
    WeakPtr(const WeakPtr& rhs);
    WeakPtr& operator=(const WeakPtr& rhs);
    ~WeakPtr();

    WeakPtr& operator=(const ACUSharedPtr_Strong<ManagedObjectSubcls>& strongRef);

public:
    uint64 GetHandle() const { return m_SharedBlock.get().handle; }
    SharedPtrNew<ManagedObjectSubcls>& GetSharedBlock() { return (SharedPtrNew<ManagedObjectSubcls>&)m_SharedBlock.get(); }
    // Resets to empty and _does_not_ decrement refcount.
    SharedPtrNew<ManagedObjectSubcls>& Release() { auto& freeSharedBlock = GetSharedBlock(); m_SharedBlock = *g_emptySharedBlock; return freeSharedBlock; }
    ACUSharedPtr_Strong<ManagedObjectSubcls> Lock() const { return ACUSharedPtr_Strong<ManagedObjectSubcls>((SharedPtrNew<ManagedObjectSubcls>&)m_SharedBlock.get()); }
    // Resets to empty and decrements refcount.
    void Reset();
private:
    // In the game, the pointer to a shared block is never a `nullptr`.
    // When a shared ptr is reset to "empty", it just points to `g_emptySharedBlock` (== 0x14525BB58).
    // It's an optimisation thing, I guess, to avoid nullptr-checking when increasing/decreasing the refcount.
    std::reference_wrapper<SharedBlock> m_SharedBlock;
};
assert_sizeof(WeakPtr<ManagedObject>, 8);


template<class ManagedObjectSubcls>
inline void WeakPtr<ManagedObjectSubcls>::Reset()
{
    SharedBlock& sharedBlock = m_SharedBlock.get();
    m_SharedBlock = *g_emptySharedBlock;
    sharedBlock.DecrementWeakRefcount();
}
template<class ManagedObjectSubcls>
inline WeakPtr<ManagedObjectSubcls>::WeakPtr(SharedPtrNew<ManagedObjectSubcls>& sharedBlock)
    : m_SharedBlock(static_cast<SharedBlock&>(sharedBlock))
{
    sharedBlock.IncrementWeakRefcount();
}
template<class ManagedObjectSubcls>
inline WeakPtr<ManagedObjectSubcls>::WeakPtr(const WeakPtr& rhs)
    : m_SharedBlock(rhs.m_SharedBlock)
{
    m_SharedBlock.get().IncrementWeakRefcount();
}
template<class ManagedObjectSubcls>
inline WeakPtr<ManagedObjectSubcls>& WeakPtr<ManagedObjectSubcls>::operator=(const WeakPtr& rhs)
{
    SharedBlock& prevSharedBlock = m_SharedBlock.get();
    m_SharedBlock = rhs.m_SharedBlock;
    m_SharedBlock.get().IncrementWeakRefcount();
    prevSharedBlock.DecrementWeakRefcount();
    return *this;
}
template<class ManagedObjectSubcls>
inline WeakPtr<ManagedObjectSubcls>::WeakPtr(uint64 handle)
    : m_SharedBlock(FindOrMakeSharedBlockByHandleAndIncrementWeakrefcount(handle))
{
}
template<class ManagedObjectSubcls>
inline WeakPtr<ManagedObjectSubcls>::WeakPtr()
    : m_SharedBlock(*g_emptySharedBlock)
{
}
template<class ManagedObjectSubcls>
inline WeakPtr<ManagedObjectSubcls>::~WeakPtr()
{
    m_SharedBlock.get().DecrementWeakRefcount();
}
template<class ManagedObjectSubcls>
WeakPtr<ManagedObjectSubcls>& WeakPtr<ManagedObjectSubcls>::operator=(const ACUSharedPtr_Strong<ManagedObjectSubcls>& strongRef)
{
    SharedBlock& prevSharedBlock = m_SharedBlock.get();
    m_SharedBlock = strongRef.m_SharedBlock;
    m_SharedBlock.get().IncrementWeakRefcount();
    prevSharedBlock.DecrementWeakRefcount();
    return *this;
}
} // namespace ACU

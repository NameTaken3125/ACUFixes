#pragma once

#include "ACU/basic_types.h"
#include "ACU/SharedPtr.h"

SharedBlock& FindOrMakeSharedBlockByHandleAndIncrementWeakrefcount(uint64 handle);
SharedBlock& FindOrMakeSharedBlockByHandleAndIncrementStrongRefcount(uint64 handle);
SharedBlock& FindSharedBlockByObjectAndIncrementWeakrefcount(ManagedObject& manObj);
SharedBlock& FindSharedBlockByObjectAndIncrementStrongRefcount(ManagedObject& manObj);




SharedBlock* const g_emptySharedBlock = (SharedBlock*)0x14525BB58;

namespace ACU {
template<class ManagedObjectSubcls> class WeakRef;

template<class ManagedObjectSubcls>
class StrongRef
{
public:
    // "Empty" shared ptr.
    StrongRef();
    StrongRef(uint64 managedObjectHandle);
    StrongRef(SharedPtrNew<ManagedObjectSubcls>& sharedBlock);
    StrongRef(const StrongRef& rhs);
    StrongRef& operator=(const StrongRef& rhs);
    ~StrongRef();

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
    friend ACU::WeakRef<ManagedObjectSubcls>;
};
assert_sizeof(StrongRef<ManagedObject>, 8);


template<class ManagedObjectSubcls>
inline void StrongRef<ManagedObjectSubcls>::Reset()
{
    SharedBlock& sharedBlock = m_SharedBlock.get();
    m_SharedBlock = *g_emptySharedBlock;
    sharedBlock.DecrementStrongRefcount();
}
template<class ManagedObjectSubcls>
inline StrongRef<ManagedObjectSubcls>::StrongRef(SharedPtrNew<ManagedObjectSubcls>& sharedBlock)
    : m_SharedBlock(static_cast<SharedBlock&>(sharedBlock))
{
    sharedBlock.IncrementStrongRefcount();
}
template<class ManagedObjectSubcls>
inline StrongRef<ManagedObjectSubcls>::StrongRef(const StrongRef& rhs)
    : m_SharedBlock(rhs.m_SharedBlock)
{
    m_SharedBlock.get().IncrementStrongRefcount();
}
template<class ManagedObjectSubcls>
inline StrongRef<ManagedObjectSubcls>& StrongRef<ManagedObjectSubcls>::operator=(const StrongRef& rhs)
{
    SharedBlock& prevSharedBlock = m_SharedBlock.get();
    m_SharedBlock = rhs.m_SharedBlock;
    m_SharedBlock.get().IncrementStrongRefcount();
    prevSharedBlock.DecrementStrongRefcount();
    return *this;
}
template<class ManagedObjectSubcls>
inline StrongRef<ManagedObjectSubcls>::StrongRef(uint64 handle)
    : m_SharedBlock(FindOrMakeSharedBlockByHandleAndIncrementStrongRefcount(handle))
{
}
template<class ManagedObjectSubcls>
inline StrongRef<ManagedObjectSubcls>::StrongRef()
    : m_SharedBlock(*g_emptySharedBlock)
{
}
template<class ManagedObjectSubcls>
inline StrongRef<ManagedObjectSubcls>::~StrongRef()
{
    m_SharedBlock.get().DecrementStrongRefcount();
}

template<class ManagedObjectSubcls>
class WeakRef
{
public:
    // "Empty" shared ptr.
    WeakRef();
    WeakRef(uint64 managedObjectHandle);
    WeakRef(SharedPtrNew<ManagedObjectSubcls>& sharedBlock);
    WeakRef(const WeakRef& rhs);
    WeakRef& operator=(const WeakRef& rhs);
    ~WeakRef();

    WeakRef& operator=(const StrongRef<ManagedObjectSubcls>& strongRef);

public:
    uint64 GetHandle() const { return m_SharedBlock.get().handle; }
    SharedPtrNew<ManagedObjectSubcls>& GetSharedBlock() { return (SharedPtrNew<ManagedObjectSubcls>&)m_SharedBlock.get(); }
    // Resets to empty and _does_not_ decrement refcount.
    SharedPtrNew<ManagedObjectSubcls>& Release() { auto& freeSharedBlock = GetSharedBlock(); m_SharedBlock = *g_emptySharedBlock; return freeSharedBlock; }
    StrongRef<ManagedObjectSubcls> Lock() const { return StrongRef<ManagedObjectSubcls>((SharedPtrNew<ManagedObjectSubcls>&)m_SharedBlock.get()); }
    // Resets to empty and decrements refcount.
    void Reset();
private:
    // In the game, the pointer to a shared block is never a `nullptr`.
    // When a shared ptr is reset to "empty", it just points to `g_emptySharedBlock` (== 0x14525BB58).
    // It's an optimisation thing, I guess, to avoid nullptr-checking when increasing/decreasing the refcount.
    std::reference_wrapper<SharedBlock> m_SharedBlock;
};
assert_sizeof(WeakRef<ManagedObject>, 8);


template<class ManagedObjectSubcls>
inline void WeakRef<ManagedObjectSubcls>::Reset()
{
    SharedBlock& sharedBlock = m_SharedBlock.get();
    m_SharedBlock = *g_emptySharedBlock;
    sharedBlock.DecrementWeakRefcount();
}
template<class ManagedObjectSubcls>
inline WeakRef<ManagedObjectSubcls>::WeakRef(SharedPtrNew<ManagedObjectSubcls>& sharedBlock)
    : m_SharedBlock(static_cast<SharedBlock&>(sharedBlock))
{
    sharedBlock.IncrementWeakRefcount();
}
template<class ManagedObjectSubcls>
inline WeakRef<ManagedObjectSubcls>::WeakRef(const WeakRef& rhs)
    : m_SharedBlock(rhs.m_SharedBlock)
{
    m_SharedBlock.get().IncrementWeakRefcount();
}
template<class ManagedObjectSubcls>
inline WeakRef<ManagedObjectSubcls>& WeakRef<ManagedObjectSubcls>::operator=(const WeakRef& rhs)
{
    SharedBlock& prevSharedBlock = m_SharedBlock.get();
    m_SharedBlock = rhs.m_SharedBlock;
    m_SharedBlock.get().IncrementWeakRefcount();
    prevSharedBlock.DecrementWeakRefcount();
    return *this;
}
template<class ManagedObjectSubcls>
inline WeakRef<ManagedObjectSubcls>::WeakRef(uint64 handle)
    : m_SharedBlock(FindOrMakeSharedBlockByHandleAndIncrementWeakrefcount(handle))
{
}
template<class ManagedObjectSubcls>
inline WeakRef<ManagedObjectSubcls>::WeakRef()
    : m_SharedBlock(*g_emptySharedBlock)
{
}
template<class ManagedObjectSubcls>
inline WeakRef<ManagedObjectSubcls>::~WeakRef()
{
    m_SharedBlock.get().DecrementWeakRefcount();
}
template<class ManagedObjectSubcls>
WeakRef<ManagedObjectSubcls>& WeakRef<ManagedObjectSubcls>::operator=(const StrongRef<ManagedObjectSubcls>& strongRef)
{
    SharedBlock& prevSharedBlock = m_SharedBlock.get();
    m_SharedBlock = strongRef.m_SharedBlock;
    m_SharedBlock.get().IncrementWeakRefcount();
    prevSharedBlock.DecrementWeakRefcount();
    return *this;
}
} // namespace ACU

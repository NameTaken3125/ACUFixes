#pragma once

#include "ACU/ManagedPtrs/ManagedPtrs.h"
constexpr uint64 handle_MaximumInACU = 272054823035; // => LargeMesh_LV_Ground_07A_LOD0_Chunk_0047_MeshShape/LargeMesh_LV_Ground_07A_LOD0_Chunk_0047_MeshShape.MeshShape

class NewHandlesFactory
{
public:
    template<typename ManagedObjectSubcls>
    ACU::StrongRef<ManagedObjectSubcls> AllocateNewHandle();
    template<typename ManagedObjectSubcls, typename CallableThatInitializesTheObject>
    ACU::StrongRef<ManagedObjectSubcls> CreateNewManagedObject(CallableThatInitializesTheObject initialize);
private:
    uint64 m_NextFreeHandle = handle_MaximumInACU + 1;
};
extern NewHandlesFactory g_NewHandlesFactory;

template<typename ManagedObjectSubcls>
ACU::StrongRef<ManagedObjectSubcls> NewHandlesFactory::AllocateNewHandle()
{
    uint64 newHandle;
    ACU::StrongRef<ManagedObjectSubcls> newSharedBlock;
    do
    {
        newHandle = m_NextFreeHandle++;
        newSharedBlock = ACU::StrongRef<ManagedObjectSubcls>(newHandle);
    } while (newSharedBlock.GetSharedBlock().manObj); // Handle already taken.
    return newSharedBlock;
}

#include "ACU_DefineNativeFunction.h"
DEFINE_GAME_FUNCTION(UsesTypeInfoCreate, 0x1426EB1A0, ManagedObject*, __fastcall, (__int64 p_handle, __int64 a2, TypeInfo* a3));
DEFINE_GAME_FUNCTION(JoinManagedObjectAndHandle_mb, 0x142714230, ManagedObject*, __fastcall, (__int64 p_handle, ManagedObject* p_manObj));
template<typename ManagedObjectSubcls, typename CallableThatInitializesTheObject>
ACU::StrongRef<ManagedObjectSubcls> NewHandlesFactory::CreateNewManagedObject(CallableThatInitializesTheObject initialize)
{
    ACU::StrongRef<ManagedObjectSubcls> newSharedBlock = this->AllocateNewHandle<ManagedObjectSubcls>();
    uint64 newHandle = newSharedBlock.GetSharedBlock().handle;
    // Managed objects lock is unnecessarily released here.
    TypeInfo& ti = ManagedObjectSubcls::GetTI();
    ManagedObjectSubcls* newManObj = static_cast<ManagedObjectSubcls*>(UsesTypeInfoCreate(newHandle, 0, &ti));

    initialize(*newManObj, newHandle);

    JoinManagedObjectAndHandle_mb(newHandle, newManObj);
    return newSharedBlock;
}

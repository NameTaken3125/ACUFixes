#include "pch.h"

#include "Hack_GunRange.h"

#include "ACU/WeaponComponent.h"
#include "ACU/Entity.h"
#include "ACU_DefineNativeFunction.h"




#include "ACU/ManagedObject.h"
#include "ACU/SharedPtr.h"
class AllManagedObjects_mb
{
public:

    // @helper_functions
    static AllManagedObjects_mb& GetSingleton() { return *(AllManagedObjects_mb*)0x14525BB70; }
};
DEFINE_GAME_FUNCTION(ManagedObjectsList__GetByHandle_mb, 0x1426EAF00, SharedPtrNew<ManagedObject>*, __fastcall, (AllManagedObjects_mb* objList, __int64 p_managedObjHandle_mb_opt, ManagedObject* a3));
DEFINE_GAME_FUNCTION(ManagedObjectsList__FreeObject_mb, 0x1426EC8E0, void, __fastcall, (AllManagedObjects_mb* objList, SharedPtrNew<ManagedObject>* a2));
template<class ManagedObjectSubcls>
void ReleaseSharedPtr(SharedPtrNew<ManagedObjectSubcls>& sharedPtr)
{
    ManagedObject* obj = sharedPtr.GetPtr();
    if (!_InterlockedDecrement(&sharedPtr.refCount))
    {
        // Refcount (weak refcount maybe?) reached 0.
        if (obj && !(sharedPtr.dword_C & 0xFFFFFF))
        {
            SharedPtrNew<ManagedObject>* asManagedObjPtr = (SharedPtrNew<ManagedObject>*) & sharedPtr;
            ManagedObjectsList__FreeObject_mb(&AllManagedObjects_mb::GetSingleton(), asManagedObjPtr);
        }
    }
}

DEFINE_GAME_FUNCTION(Entity__Get_Human1C8, 0x140C17A50, void*, __fastcall, (Entity* entity));
DEFINE_GAME_FUNCTION(Entity__Get_WeaponComponent, 0x140C1B7E0, WeaponComponent*, __fastcall, (Entity* weaponEntity));
DEFINE_GAME_FUNCTION(Human1C8__GetCurrentRangedWeaponShared_mb, 0x140C15130, SharedPtrNew<Entity>**, __fastcall, (void* human1C8, SharedPtrNew<Entity>** p_out, char p_0forRangedWeapon));
WeaponComponent* FindCurrentRangedWeaponComponent(Entity& player)
{
    // The recreation of `1409E7C10`: `Entity::GetCurrentRangedWeapon_NetFightWeapon_18()`.

    SharedPtrNew<Entity>* foundRangedWeapon = nullptr;
    void* human1C8 = Entity__Get_Human1C8(&player);
    if (!human1C8) { return nullptr; }
    Human1C8__GetCurrentRangedWeaponShared_mb(human1C8, &foundRangedWeapon, 0);
    Entity* wpnEntity = foundRangedWeapon->GetPtr();
    ReleaseSharedPtr(*foundRangedWeapon);
    if (!wpnEntity) { return nullptr; }
    return Entity__Get_WeaponComponent(wpnEntity);
}
std::optional<float> CalculateAdjustedRangeForCurrentRangedWeapon(Entity& player)
{
    WeaponComponent* wpnCpnt = FindCurrentRangedWeaponComponent(player);
    if (!wpnCpnt) { return {}; }
    const bool isGuillotineGun = wpnCpnt->weaponCpntType == WeaponComponentType::GuillotineGun;
    if (isGuillotineGun)
    {
        return 20.0f;
    }
    NetFightWeapon* netFightWpn = wpnCpnt->netFightWeapon->GetPtr();
    if (!netFightWpn) { return {}; }
    NetFightWeapon_18* rangedData = netFightWpn->magazineData;
    if (!rangedData) { return {}; }
    return rangedData->range;
}
void WhenSearchingForRangedNPCTargetGettingRangedWeaponData_AdjustForWeaponType(AllRegisters* params)
{
    Entity* player = (Entity*)params->rcx_;
    float* rangeOut = (float*)params->rbx_;
    std::optional<float> range = CalculateAdjustedRangeForCurrentRangedWeapon(*player);
    if (range)
    {
        *rangeOut = *range;
    }
}
GuillotineGunRange::GuillotineGunRange()
{
    uintptr_t whenSearchingForRangedNPCTargetGettingRangedWeaponData = 0x141A057E9;
    uintptr_t sameFunctionEpilogue = 0x141A057F8;
    PresetScript_CCodeInTheMiddle(whenSearchingForRangedNPCTargetGettingRangedWeaponData, 5,
        WhenSearchingForRangedNPCTargetGettingRangedWeaponData_AdjustForWeaponType, sameFunctionEpilogue, false);
}

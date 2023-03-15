#include "pch.h"

#include "Cheat_BatlampOfFranciade.h"

#include "ACU/InventoryItemSettings.h"
#include "ACU/AvatarGearManager.h"
constexpr uint64 magicalLanternHandle = 0x000000238FD23D0F;
constexpr uint64 oldAndBustedLanternHandle = 0x000000238FD23D13;
AvatarGear* FindMagicalLanternGear(AvatarGearManager& agm)
{
    for (auto rit = agm.gears_1C_hasLanternAtEnd.rbegin(), rend = agm.gears_1C_hasLanternAtEnd.rend(); rit != rend; rit++)
    {
        if ((*rit)->inventoryItemSettings->handle == magicalLanternHandle)
        {
            return *rit;
        }
    }
    return nullptr;
}
void WhenOutOfMissionEquippingTheLantern_SubstituteNormalLanternWithTheMagicalOne(AllRegisters* params)
{
    AvatarGearManager* agm = *(AvatarGearManager**)params->rbx_;
    AvatarGear*& gearToBeEquipped = (AvatarGear*&)*params->rax_;
    if (gearToBeEquipped->inventoryItemSettings->handle == oldAndBustedLanternHandle)
    {
        AvatarGear* magicalLanternGear = FindMagicalLanternGear(*agm);
        if (magicalLanternGear)
        {
            gearToBeEquipped = magicalLanternGear;
        }
    }
}
BatlampOfFrancide::BatlampOfFrancide()
{
    uintptr_t whenOutOfMissionEquippingTheLantern = 0x140D2B330;
    PresetScript_CCodeInTheMiddle(
        whenOutOfMissionEquippingTheLantern, 6,
        WhenOutOfMissionEquippingTheLantern_SubstituteNormalLanternWithTheMagicalOne, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

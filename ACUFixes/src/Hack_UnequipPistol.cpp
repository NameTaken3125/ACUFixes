#include "pch.h"

#include "Hack_UnequipPistol.h"

#include "ACU_DefineNativeFunction.h"

#include "ACU/ButtonComponent.h"
#include "ACU/GearPage.h"
#include "ACU/AvatarGearManager.h"
#include "ACU/SharedPtr.h"
class PanelPageHelper
{
public:
    char pad_0000[56]; //0x0000
    uint8 isPickedASkillWaitingToConfirm; //0x0038
    char pad_0039[59]; //0x0039
    SmallArray<ButtonComponent_230> arrCallbacks74; //0x0074
    char pad_0080[36]; //0x0080
    SmallArray<ButtonComponent_230> arrCallbacksA4; //0x00A4
    char pad_00B0[96]; //0x00B0
}; //Size: 0x0110
assert_sizeof(PanelPageHelper, 0x110);


class ScrollableWidgetComponent : public WidgetComponent
{
public:
}; //Size: 0x0210
assert_sizeof(ScrollableWidgetComponent, 0x210);

class UITweener : public ManagedObject
{
public:
    char pad_0010[24]; //0x0010
}; //Size: 0x0028
assert_sizeof(UITweener, 0x28);

class ScrollingListComponent : public ScrollableWidgetComponent
{
public:
    char pad_0210[104]; //0x0210
    SharedPtrNew<EntityGroup>* shared_entityGroupWithLayoutCpnt; //0x0278
    char pad_0280[88]; //0x0280
    UITweener uiTweener; //0x02D8
    void* arrUIEventReactions; //0x0300
    char pad_0308[264]; //0x0308
}; //Size: 0x0410
assert_offsetof(ScrollingListComponent, uiTweener, 0x2D8);
assert_offsetof(ScrollingListComponent, arrUIEventReactions, 0x300);
assert_sizeof(ScrollingListComponent, 0x410);

PanelPageHelper* DynamicCast_PanelPageHelper(void* obj)
{
    constexpr uint64 vtbl_PanelPageHelper = 0x142EEC010;
    if (*(uint64*)obj == vtbl_PanelPageHelper)
    {
        return (PanelPageHelper*)obj;
    }
    return nullptr;
}
GearPage* DynamicCast_GearPage(void* obj)
{
    constexpr uint64 vtbl_GearPage = 0x142EDF810;
    if (*(uint64*)obj == vtbl_GearPage)
    {
        return (GearPage*)obj;
    }
    return nullptr;
}

class GearUIModifier;
class GearUIInfo : public ManagedObject
{
public:
    char pad_0010[24]; //0x0010
    GearPage* gearPage; //0x0028
    AvatarGear* gear; //0x0030
    SmallArray<GearUIModifier*> gearUIModifiers; //0x0038
    char pad_0044[12]; //0x0044
    void* shared_textureMapSpec_mb; //0x0050
    char pad_0058[192]; //0x0058
}; //Size: 0x0118
assert_sizeof(GearUIInfo, 0x118);
DEFINE_GAME_FUNCTION(CastTo_GearUIInfo, 0x14086E030, GearUIInfo*, __fastcall, (ManagedObject* obj));
static bool IsGearUIInfoCorrespondingToAPistol(GearUIInfo& gearInfo)
{
    return
        gearInfo.gear
        && gearInfo.gear->gearType == 5
        && gearInfo.gear->subtypeIfMeleePistolOrLantern == 5;
}
static bool IsTheButtonSupposedToUnequipPistol(ButtonComponent& button)
{
    if (!button.arrCallbacksWhenEnabled_mb.size)
    {
        return false;
    }
    void* receiver1 = button.arrCallbacksWhenEnabled_mb[0].GetReceiver();
    PanelPageHelper* pph = DynamicCast_PanelPageHelper(receiver1);
    if (!pph)
    {
        return false;
    }
    if (!pph->arrCallbacks74.size)
    {
        return false;
    }
    void* receiver2 = pph->arrCallbacks74[0].GetReceiver();
    GearPage* gearPage = DynamicCast_GearPage(receiver2);
    if (!gearPage)
    {
        return false;
    }
    ManagedObject* gearUIInfo_mb = gearPage->scrollListCpnt_mb->base20.Unk_000_GetCurrentlySelectedGearUIInfo_mb();
    GearUIInfo* gearUIinfo = CastTo_GearUIInfo(gearUIInfo_mb);
    if (!gearUIinfo)
    {
        return false;
    }
    const bool isGearAPistol = IsGearUIInfoCorrespondingToAPistol(*gearUIinfo);
    return isGearAPistol;
}
void WhenJustTriedToPressUIButton_EnabledButtonIfAttemptToEquip(AllRegisters* params)
{
    ButtonComponent* button = (ButtonComponent*)params->rcx_;
    if (button->isNotGreyedOut_mb)
    {
        // Button is already enabled.
        return;
    }
    if (IsTheButtonSupposedToUnequipPistol(*button))
    {
        button->isNotGreyedOut_mb = true;
    }
}
void WhenPressedAnEnabledEquipGearButton_AllowProcessingIfPistol(AllRegisters* params)
{
    void* gearPage3E0 = (void*)params->rdi_;
    GearUIInfo* gearInfo = (GearUIInfo*)params->rdx_;
    using SomeCallDuringEquipPistolFromUI_fnt = bool(*)(void* gearPage3E0, GearUIInfo* gearInfo);
    SomeCallDuringEquipPistolFromUI_fnt someCall = (SomeCallDuringEquipPistolFromUI_fnt)params->r8_;
    const bool someCallResult = someCall(gearPage3E0, gearInfo);
    params->GetRAX() = someCallResult;
    if (someCallResult)
    {
        return;
    }
    // The "Try to equip Gear" function will return immediately unless I prevent that.
    // If the gear that is attempted to be equipped is a Pistol, then I allow the further processing.
    if (!IsGearUIInfoCorrespondingToAPistol(*gearInfo))
    {
        return;
    }
    params->GetRAX() = 1;
}
DEFINE_GAME_FUNCTION(GearUIInfo__IsGearEquipped, 0x140889020, char, __fastcall, (GearUIInfo* gearInfo));
void WhenTryingToEquipGear_CheckIfTryingToREEQUIPpistol(AllRegisters* params)
{
    GearUIInfo* gearInfo = (GearUIInfo*)params->rbx_;
    const bool isGearAlreadyEquipped = GearUIInfo__IsGearEquipped(gearInfo);
    params->GetRAX() = isGearAlreadyEquipped;
    if (!isGearAlreadyEquipped)
    {
        return;
    }
    if (IsGearUIInfoCorrespondingToAPistol(*gearInfo))
    {
        params->GetRAX() = 0;
    }
}
DEFINE_GAME_FUNCTION(AvatarGearManager__UnequipOutfit_mb, 0x140D40610, void, __fastcall, (AvatarGearManager* a1, AvatarGear* p_gear));
DEFINE_GAME_FUNCTION(AvatarGearManager__EquipGear_mb, 0x140D2B240, void, __fastcall, (AvatarGearManager* a1, AvatarGear* p_gear));
void WhenAboutToSuccessfullyEquipGear_UnequipPistolInsteadOfEquipping(AllRegisters* params)
{
    GearUIInfo* gearInfo = (GearUIInfo*)params->rbx_;
    AvatarGearManager* agm = (AvatarGearManager*)params->rcx_;
    AvatarGear* gear = (AvatarGear*)params->rdx_;
    if (!IsGearUIInfoCorrespondingToAPistol(*gearInfo))
    {
        AvatarGearManager__EquipGear_mb(agm, gear);
        return;
    }
    // It's a pistol. If already equipped, unequip.
    if (GearUIInfo__IsGearEquipped(gearInfo))
    {
        AvatarGearManager__UnequipOutfit_mb(agm, gear);
    }
    else
    {
        AvatarGearManager__EquipGear_mb(agm, gear);
    }
}

PreventAutomaticReequipPistol::PreventAutomaticReequipPistol()
{
    uintptr_t whenBestPistolAboutToBeSelectedAfterSwitchingFromTwohandedRangedWeaponToNonrangedMelee = 0x140D2B613;
    PresetScript_NOP(whenBestPistolAboutToBeSelectedAfterSwitchingFromTwohandedRangedWeaponToNonrangedMelee, 5);
}
UnequipPistol::UnequipPistol()
{
    uintptr_t whenJustTriedToPressUIButton = 0x1407A3D0F;
    PresetScript_CCodeInTheMiddle(whenJustTriedToPressUIButton, 7,
        WhenJustTriedToPressUIButton_EnabledButtonIfAttemptToEquip, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenPressedAnEnabledEquipGearButton = 0x140854C9D;
    PresetScript_CCodeInTheMiddle(whenPressedAnEnabledEquipGearButton, 6,
        WhenPressedAnEnabledEquipGearButton_AllowProcessingIfPistol, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    uintptr_t whenTryingToEquipGearCheckingIfAlreadyEquipped = 0x140854CC9;
    PresetScript_CCodeInTheMiddle(whenTryingToEquipGearCheckingIfAlreadyEquipped, 5,
        WhenTryingToEquipGear_CheckIfTryingToREEQUIPpistol, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    uintptr_t whenAboutToSuccessfullyEquipGear = 0x140854D26;
    PresetScript_CCodeInTheMiddle(whenAboutToSuccessfullyEquipGear, 5,
        WhenAboutToSuccessfullyEquipGear_UnequipPistolInsteadOfEquipping, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
}
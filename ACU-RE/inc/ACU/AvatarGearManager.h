#pragma once

#include "ACU/ManagedObject.h"
#include "ACU/SharedPtr.h"
#include "ACU/SmallArray.h"
#include "ACU/UIString.h"

class InventoryItemSettings;

class TextureMapSpec;
class AvatarGearModifier;

class AvatarGear : public Object
{
public:
    char pad_0008[8]; //0x0008
    SharedPtrNew<InventoryItemSettings>* inventoryItemSettings; //0x0010
    SharedPtrNew<InventoryItemSettings>* invItemSett_18; //0x0018
    char pad_0020[16]; //0x0020
    SharedPtrNew<TextureMapSpec>* shared_textureMapSpec; //0x0030
    SmallArray<AvatarGearModifier*> gearModifiers_38; //0x0038
    SmallArray<AvatarGearModifier*> gearModifiers_44; //0x0044
    char pad_0050[4]; //0x0050
    uint32 price; //0x0054
    char pad_0058[8]; //0x0058
    uint32 upgradePrice; //0x0060
    char pad_0064[4]; //0x0064
    UIString uiString_gearName; //0x0068
    UIString uiString_gearDescription; //0x006C
    UIString uiString_requirementsLine2; //0x0070 // As in "THE TOURNAMENT" if the Requirements == "Co-op Mission\nTHE TOURNAMENT" or "Club Competition" if Requirements == "Play\nClub Competition"
    UIString uiString_requirementsLine1; //0x0074 // As in "Co-op Mission" if the Requirements == "Co-op Mission\nTHE TOURNAMENT" or "Play" if Requirements == "Play\nClub Competition"
    char pad_0078[32]; //0x0078
}; //Size: 0x0098
assert_offsetof(AvatarGear, inventoryItemSettings, 0x10);
assert_sizeof(AvatarGear, 0x98);


#include "ACU/AvatarGearModifierStats.h"
#include "ACU/SmallArray.h"
class AvatarGearManager : public ManagedObject
{
public:
    // @members
    SmallArray<AvatarGear*> gears_10_shopGears_mb; //0x0010
    SmallArray<AvatarGear*> gears_1C_hasLanternAtEnd; //0x001C
    char pad_28[0x38-0x28]; //0x0028
    AvatarGear gear_38; //0x0038
    AvatarGear gear_D0; //0x00D0
    AvatarGear gear_168; //0x0168
    AvatarGear gear_200; //0x0200
    AvatarGear gear_298; //0x0298
    AvatarGear gear_330; //0x0330
    char pad_03C8[8]; //0x03C8
    AvatarGearModifierStats gearModifierStats; //0x03D0
    char pad_0500[32]; //0x0500

    // @helper_functions
    static AvatarGearManager* GetSingleton();
}; //Size: 0x0520
assert_sizeof(AvatarGearManager, 0x0520);

#pragma once

#include "ManagedObject.h"
#include "UIString.h"
#include "SharedPtr.h"
#include "SmallArray.h"

class InventoryItemSettings;

class TextureMapSpec;
class AvatarGearModifier;
class Animation;

class AvatarGear : public Object
{
public:
    uint32 gearType; //0x0008
    uint32 subtypeIfMeleePistolOrLantern; //0x000C
    SharedPtrNew<InventoryItemSettings>* inventoryItemSettings; //0x0010
    SharedPtrNew<InventoryItemSettings>* invItemSett_18; //0x0018
    char pad_0020[8]; //0x0020
    SharedPtrNew<Animation>* Animation; //0x0028 The custom animation that will be played when this gear is viewed in Equipment menu.
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

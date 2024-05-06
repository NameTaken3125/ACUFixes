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
    // @members
    uint32 gearType; //0x0008
    uint32 subtypeIfMeleePistolOrLantern; //0x000C
    SharedPtrNew<InventoryItemSettings>* inventoryItemSettings; //0x0010
    SharedPtrNew<InventoryItemSettings>* invItem_upgrade; //0x0018
    char pad_0020[8]; //0x0020
    SharedPtrNew<Animation>* Animation; //0x0028 The custom animation that will be played when this gear is viewed in Equipment menu.
    SharedPtrNew<TextureMapSpec>* shared_textureMapSpec; //0x0030
    SmallArray<AvatarGearModifier*> Modifiers; //0x0038
    SmallArray<AvatarGearModifier*> additionalModifiersAfterUpgrade; //0x0044
    char pad_0050[4]; //0x0050
    uint32 price; //0x0054
    char pad_0058[4]; //0x0058
    uint32 dword_5C; //0x005C
    uint32 upgradePrice; //0x0060
    uint32 dword_64; //0x0064
    UIString uiString_gearName; //0x0068
    UIString uiString_gearDescription; //0x006C
    UIString uiString_requirementsLine2; //0x0070 // As in "THE TOURNAMENT" if the Requirements == "Co-op Mission\nTHE TOURNAMENT" or "Club Competition" if Requirements == "Play\nClub Competition"
    UIString uiString_requirementsLine1; //0x0074 // As in "Co-op Mission" if the Requirements == "Co-op Mission\nTHE TOURNAMENT" or "Play" if Requirements == "Play\nClub Competition"
    uint32 damageOrQuality_determinesBestAvailable; //0x0078
    uint32 dword_7C; //0x007C
    uint32 dword_80; //0x0080
    uint32 dword_84; //0x0084
    uint32 dword_88; //0x0088
    char pad_008C[1]; //0x008C
    uint8 bool_8D; //0x008D
    uint8 bool_8E; //0x008E
    uint8 IsHidden; //0x008F
    uint8 IsInvisible; //0x0090
    uint8 bool_91; //0x0091
    char pad_0092[6]; //0x0092

    // @helper_functions
    static TypeInfo& GetTI() { return *(TypeInfo*)0x143E232B0; }
}; //Size: 0x0098
assert_offsetof(AvatarGear, inventoryItemSettings, 0x10);
assert_sizeof(AvatarGear, 0x98);

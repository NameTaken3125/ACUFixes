#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"


class GearUIModifier;
class GearPage;
class AvatarGear;

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

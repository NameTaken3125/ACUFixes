#pragma once

#include "WorldComponent.h"
#include "SmallArray.h"

class HUDModule;
class UIWorldComponent : public WorldComponent
{
public:
    SmallArray<HUDModule*> hudModules; //0x0020
    char pad_002C[4]; //0x0028
}; //Size: 0x0030
assert_sizeof(UIWorldComponent, 0x30);

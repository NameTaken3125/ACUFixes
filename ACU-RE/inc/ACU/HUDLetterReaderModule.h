#pragma once

#include "HUDModule.h"
class HUDLetterReaderModule : public HUDModule
{
public:
    char pad_0090[200]; //0x0090
    uint8 isVisible_158; //0x0158
    char pad_0159[7]; //0x0159
}; //Size: 0x0160
assert_offsetof(HUDLetterReaderModule, isVisible_158, 0x158);
assert_sizeof(HUDLetterReaderModule, 0x160);

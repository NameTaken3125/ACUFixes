#pragma once

#include "basic_types.h"
#include "SmallArray.h"

class StatechartDesc;
class Statechart_60_ChildStatechartsArray;
class Statechart_88;

class Statechart
{
public:
    void* vtbl8; //0x0000
    char pad_0008[24]; //0x0008
    StatechartDesc* statechartDesc; //0x0020
    char pad_0028[24]; //0x0028
    Statechart* parentStatechart_mb; //0x0040
    char pad_0048[24]; //0x0048
    //Statechart_60_ChildStatechartsArray* childStatecharts_mb_toHealth_60; //0x0060
    SmallArray<Statechart*> childStatecharts_mb_toHealth_60; //0x0060
    char pad_006C[28]; //0x006C
    Statechart_88* toNodes88; //0x0088
    char pad_0090[24]; //0x0090
}; //Size: 0x00A8
assert_sizeof(Statechart, 0x00A8);

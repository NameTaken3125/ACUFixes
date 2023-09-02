#pragma once

#include "AIAction.h"

class ActionWeatherWindSelection : public AIAction
{
public:
    uint32 dword_40; //0x0040
    float flt_44; //0x0044 // cloudiness+rain TO
    float flt_48; //0x0048 // cloudiness+rain FROM
    float flt_4C; //0x004C // cloudiness+rain transition duration 1
    float flt_50; //0x0050
    float flt_54; //0x0054 // highest dryest spots wetness (includes the player's wetness)
    float flt_58; //0x0058 // lowest wettest spots wetness (puddles size)
    float flt_5C; //0x005C // wind strength
    float flt_60; //0x0060 // cloudiness+rain transition duration 2
    float flt_64; //0x0064
    uint8 byte_68; //0x0068 // doControlCloudinessPlusRain_1
    uint8 byte_69; //0x0069 // doControlCloudinessPlusRain_2
    uint8 byte_6A; //0x006A
    uint8 byte_6B; //0x006B // use flt_54==doControlDrySpots
    uint8 byte_6C; //0x006C // use flt_58==doControlPuddles
    uint8 byte_6D; //0x006D
    uint8 byte_6E; //0x006E
    uint8 byte_6F; //0x006F // use flt_5C==windStrength (flags and vegetation)
    uint8 byte_70; //0x0070 // use flt_64
    uint8 byte_71; //0x0071
    uint8 byte_72; //0x0072
    uint8 byte_73; //0x0073
    uint8 byte_74; //0x0074
    uint8 byte_75; //0x0075
    uint8 byte_76; //0x0076
    uint8 byte_77; //0x0077
    float flt_78; //0x0078 // prev flt_5C
    float flt_7C; //0x007C
    float flt_80; //0x0080 // prev flt_64
    float flt_84; //0x0084
    float flt_88; //0x0088 // prev flt_58
    uint32 dword_8C; //0x008C
}; //Size: 0x0090
assert_sizeof(ActionWeatherWindSelection, 0x90);

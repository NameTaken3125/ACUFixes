#pragma once

#include "basic_types.h"

class WeatherControls
{
public:
    // @members
    char pad_0000[36]; //0x0000
    float cloudinessCurrent; //0x0024
    float cloudinessFrom; //0x0028
    float cloudinessTo; //0x002C
    float cloudinessTransitionTime; //0x0030
    float flt_34; //0x0034
    float flt_38; //0x0038
    float cloudinessTransitionTimeLeft; //0x003C
    float timeLeftUntilCloudinessTargetRandomized; //0x0040
    uint32 semaphoreMb_dontRerandomizeCloudiness; //0x0044
    char pad_0048[4]; //0x0048
    float windStrength; //0x004C
    uint32 semaphoreMb_freezeWind; //0x0050
    char pad_0054[1]; //0x0054
    uint8 isSupposedToStartRainBut_int70greaterThan0; //0x0055
    char pad_0056[2]; //0x0056
    float rainStrengthCurrent; //0x0058
    float rainStrengthFrom; //0x005C
    float rainStrengthTo; //0x0060
    float rainStrengthTransitionTime; //0x0064
    float rainStrengthTransitionTimeLeft_68; //0x0068
    float timeLeftUntilRainStrengthTargetRandomized; //0x006C
    int32 int70_needsToBeLe0ForRain; //0x0070
    char pad_0074[4]; //0x0074
    float flt_78; //0x0078
    float drySpotsWetness; //0x007C
    float puddlesSize; //0x0080
    char pad_0084[4]; //0x0084
    float flt_88; //0x0088
    char pad_008C[8]; //0x008C
    uint32 semaphoreMb_groundWetness; //0x0094
    char pad_0098[48]; //0x0098

    // @helper_functions
    static WeatherControls& GetSingleton() { return *(WeatherControls*)0x145219DC0; }
}; //Size: 0x00C8

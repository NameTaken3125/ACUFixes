#pragma once

#include "ACU/EventListener.h"


class SoundMuteManager
{
public:
    char pad_0000[48]; //0x0000
}; //Size: 0x0030
assert_sizeof(SoundMuteManager, 0x30);

class AssassinSoundSettings;

class SoundManager : public EventListener
{
public:
    // @members
    char pad_0028[264]; //0x0028
    AssassinSoundSettings* assassinSoundSettings; //0x0130
    SoundMuteManager soundMuteManager_138; //0x0138
    char pad_0168[776]; //0x0168

    // @helper_functions
    static SoundManager& GetSingleton() { return **(SoundManager**)0x145217298; }
}; //Size: 0x0470
assert_sizeof(SoundManager, 0x470);

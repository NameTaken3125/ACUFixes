#pragma once

#include "SoundState.h"
#include "SharedPtr.h"

class ACUGlobalSoundSet;
class UISounds;


class SoundRTPC : public SoundResource
{
public:
    char pad_0010[8]; //0x0010
}; //Size: 0x0018
assert_sizeof(SoundRTPC, 0x18);

class SoundSettings : public ManagedObject
{
public:
    char pad_0010[1472]; //0x0010
}; //Size: 0x05D0
assert_sizeof(SoundSettings, 0x5D0);


class AssassinSoundSettings : public SoundSettings
{
public:
    void* sharedPtr_GlobalSoundSet; //0x05D0
    void* sharedPtr_GlobalSoundSet2; //0x05D8
    void* sharedPtr_ExoticSoundSet; //0x05E0
    void* sharedPtr_AnimalSoundSet; //0x05E8
    SharedPtrNew<UISounds>* sharedPtr_UISounds; //0x05F0
    void* sharedPtr_TreeSoundsManager; //0x05F8
    void* sharedPtr_FightFacialGroupSet; //0x0600
    SoundRTPC SoundRTPC_608; //0x0608
    void* sharedPtr_AssassinSwitchSoundSet; //0x0620
    char pad_0628[8]; //0x0628
    SoundState SoundState_630; //0x0630
    SoundState SoundState_648; //0x0648
    SharedPtrNew<ACUGlobalSoundSet>* sharedPtr_ACUGlobalSoundSet; //0x0660
    void* sharedPtr_MaterialSoundsManager; //0x0668
    char pad_0670[24]; //0x0670
    void* sharedPtr_SandboxMusicSettings; //0x0688
    char pad_0690[1608]; //0x0690
}; //Size: 0x0CD8
assert_sizeof(AssassinSoundSettings, 0xCD8);

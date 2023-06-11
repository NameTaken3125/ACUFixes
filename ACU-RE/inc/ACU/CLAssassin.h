#pragma once

#include "CLAbstract.h"
#include "Timer.h"

class Entity;
class LanterndlcComponent;
class WhenHighlightedNPCChanges;
class HasLanterndlcComponent_30;

class HasLanterndlcComponent
{
public:
    char pad_0000[16]; //0x0000
    Entity* player; //0x0010
    WhenHighlightedNPCChanges* toHighlightedNPC; //0x0018
    char pad_0020[16]; //0x0020
    HasLanterndlcComponent_30* stru_30; //0x0030
    char pad_0038[1104]; //0x0038
    Timer timer_ReholsterAfterQuickshot; //0x0488
    uint32 quickshot_isGunNotYetUnholstered; //0x04B0
    char pad_04B4[260]; //0x04B4
    Timer timer_MoneyPouchReload; //0x05B8
    LanterndlcComponent* lanternCpnt; //0x05E0
    char pad_05E8[88]; //0x05E8
    float flt_640; //0x0640
    char pad_0644[188]; //0x0644
}; //Size: 0x0700
assert_offsetof(HasLanterndlcComponent, lanternCpnt, 0x5E0);
assert_sizeof(HasLanterndlcComponent, 0x700);

class AssassinTask;
class DebugCommandsHolder;

class CLAssassin : public CLAbstract
{
public:
    // @members
    AssassinTask* assassinTask; //0x00A8
    DebugCommandsHolder* cheatCodes; //0x00B0
    HasLanterndlcComponent* hasLanternCpnt; //0x00B8

    // @helper_functions
    static CLAssassin* GetSingleton();
}; //Size: 0x00C0
assert_sizeof(CLAssassin, 0xC0);

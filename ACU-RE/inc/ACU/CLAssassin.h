#pragma once

#include "CLAbstract.h"

class Entity;
class LanterndlcComponent;

class HasLanterndlcComponent
{
public:
    char pad_0000[16]; //0x0000
    Entity* player; //0x0010
    char pad_0018[1480]; //0x0018
    LanterndlcComponent* lanternCpnt; //0x05E0
    char pad_05E8[280]; //0x05E8
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

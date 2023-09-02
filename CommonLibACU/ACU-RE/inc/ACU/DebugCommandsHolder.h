#pragma once

#include "AITask.h"
#include "SmallArray.h"

class Human;
class NPCNavigation;

class CharacterTask : public AITask
{
public:
    Human* human; //0x0020
    NPCNavigation* npcNavigation; //0x0028
}; //Size: 0x0030
assert_sizeof(CharacterTask, 0x30);

class InputContainer;
class DebugCommandsHolder;
class Clock;

class DebugCommand
{
public:
    using CommandExecute_t = void(__fastcall*)(int a1, DebugCommandsHolder* a2);

    char pad_0000[8]; //0x0000
    uint64 creationTimestamp; //0x0008
    char pad_0010[8]; //0x0010
    Clock* rootClock; //0x0018
    char pad_0020[16]; //0x0020
    CommandExecute_t fnExecute; //0x0030
    char pad_0038[16]; //0x0038
    char* textDesc; //0x0048
    char pad_0050[8]; //0x0050
    DebugCommandsHolder* cheatsHolder; //0x0058
    char pad_0060[8]; //0x0060
}; //Size: 0x0068
assert_sizeof(DebugCommand, 0x68);

class DebugCommandsHolder : public CharacterTask
{
public:
    char pad_0030[16]; //0x0030
    InputContainer* mainInputContainer; //0x0040
    char pad_0048[1924]; //0x0048
    SmallArray<DebugCommand*> arrDebugCommands; //0x07CC
    char pad_07D8[136]; //0x07D8
}; //Size: 0x0860
assert_sizeof(DebugCommandsHolder, 0x860);

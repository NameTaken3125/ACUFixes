#pragma once

#include "ManagedObject.h"

class SoundID
{
public:
    uint32 soundEvnHash; //0x0000
}; //Size: 0x0004
assert_sizeof(SoundID, 4);

class SoundResource : public Object
{
public:
    enum class ResourceType
    {
        Type_Event = 0,
        Type_Switch = 1,
        Type_SwitchGroup = 2,
        Type_State = 3,
        Type_Rtpc = 4,
        Type_AuxiliarySend = 5,
        Type_Bank = 6,
        Type_Trigger = 7,
        Type_Effect = 8,
        Type_StateGroup = 9,
        Type_Unknown = 10,
    };

    // @members
    class SoundID soundID; //0x0008
    char pad_000C[4]; //0x000C
}; //Size: 0x0010
assert_sizeof(SoundResource, 0x10);

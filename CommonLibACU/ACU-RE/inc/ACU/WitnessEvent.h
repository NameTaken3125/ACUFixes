#pragma once

#include "ManagedObject.h"
#include "SharedPtr.h"
#include "Event.h"


class SharedPtrAndSmth
{
public:
    SharedPtrNew<Entity>* shared; //0x0000
    uint64 qword_8; //0x0008
    uint64 qword_10; //0x00010
}; //Size: 0x0018
assert_sizeof(SharedPtrAndSmth, 0x18);



#include "Enum_ReactionHash_ReactToWhat.h"
class DangerEvent : public Event
{
public:
    SharedPtrAndSmth witnessWhoPlayer; //0x0010
    SharedPtrAndSmth witnessByWhomNPC; //0x0028
    uint32 responseHash; //0x0040 // I now store this in `ReactionHash_ReactToWhat` enum.
    uint32 dword_44; //0x0044
}; //Size: 0x0048
assert_sizeof(DangerEvent, 0x48);
class WitnessEvent : public DangerEvent
{
public:
}; //Size: 0x0048
assert_sizeof(WitnessEvent, 0x48);

#pragma once

#include "ManagedObject.h"

class EventListener : public ManagedObject
{
public:
    char pad_0010[24]; //0x0010
}; //Size: 0x0028
assert_sizeof(EventListener, 0x28);

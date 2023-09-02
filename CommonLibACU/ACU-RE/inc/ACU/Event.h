#pragma once

#include "ManagedObject.h"
#include "SharedPtr.h"

class Event : public Object
{
public:
    SharedPtrNew<Entity>* sharedPtr_Sender_mb; //0x0008
}; //Size: 0x0010
assert_sizeof(Event, 0x10);

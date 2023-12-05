#pragma once

#include "basic_types.h"

class AtomNodeID
{
public:
    char pad_0000[16]; //0x0000
}; //Size: 0x0010
assert_sizeof(AtomNodeID, 0x10);

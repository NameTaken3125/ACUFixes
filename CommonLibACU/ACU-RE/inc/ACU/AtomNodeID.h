#pragma once

#include "basic_types.h"

class AtomNodeID
{
public:
    byte ByteData[16]; //0x0000
}; //Size: 0x0010
assert_sizeof(AtomNodeID, 0x10);

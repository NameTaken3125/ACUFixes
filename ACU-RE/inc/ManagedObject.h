#pragma once

#include "basic_types.h"

class Object
{
public:
    char pad_0000[8]; //0x0000
}; //Size: 0x0008
assert_sizeof(Object, 8);

class ManagedObject : public Object
{
public:
    char pad_0008[8]; //0x0008
}; //Size: 0x0010
assert_sizeof(ManagedObject, 0x10);

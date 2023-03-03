#pragma once

#include "basic_types.h"

class Entity;

class SharedPtr_mb
{
public:
	Entity* entity_mb; //0x0000
	char pad_0008[16]; //0x0008
}; //Size: 0x0018
assert_sizeof(SharedPtr_mb, 0x18);

#pragma once

#include "ManagedObject.h"

class UniverseComponent : public ManagedObject
{
public:
}; //Size: 0x0010

class MandatoryUniverseComponent : public UniverseComponent
{
public:
}; //Size: 0x0010
assert_sizeof(MandatoryUniverseComponent, 0x10);

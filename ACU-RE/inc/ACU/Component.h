#pragma once

#include "ManagedObject.h"

class Entity;

class BaseComponent : public ManagedObject
{
public:
}; //Size: 0x0010
assert_sizeof(BaseComponent, 0x10);

class Component : public BaseComponent
{
public:
    Entity* entity; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020
assert_sizeof(Component, 0x20);

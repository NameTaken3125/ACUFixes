#pragma once

#include "vmath.h"

#include "ManagedObject.h"

class World;

class BaseEntity : public ManagedObject
{
public:
    // @members
    char pad_0010[16]; //0x0010
    Matrix4f mainTransform; //0x0020
    World* world; //0x0060
    char pad_0068[16]; //0x0068

    // @helper_functions
    Matrix4f& GetTransform() { return mainTransform; }
    Vector4f& GetPosition() { return *(Vector4f*)((char*)&mainTransform + 0x30); }
}; //Size: 0x0078
assert_sizeof(BaseEntity, 0x78);

#pragma once

#include "vmath/vmath.h"

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
    Vector3f& GetPosition() { return *(Vector3f*)((char*)&mainTransform + 0x30); }
    Vector3f GetDirectionRight() { return (Vector3f&)mainTransform[4 * 0]; }
    Vector3f GetDirectionForward() { return (Vector3f&)mainTransform[4 * 1]; }
}; //Size: 0x0078
assert_sizeof(BaseEntity, 0x78);

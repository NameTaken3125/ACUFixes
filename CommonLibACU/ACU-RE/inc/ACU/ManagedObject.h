#pragma once

#include "basic_types.h"

class DeserializationStream;
class TypeInfo;

class Object
{
public:
    virtual void Unk000();
    virtual void Unk008();
    virtual void Unk010_Deserialize(DeserializationStream* deserializationStream);
    virtual Object* Unk018_Clone(uint64 a2, uint32 a3);
    virtual void Unk020();
    virtual TypeInfo& Unk028_GetTI();
}; //Size: 0x0008
assert_sizeof(Object, 8);

class ManagedObject : public Object
{
public:
    uint32 bits_8_0_0x1C : 0x1C;
    uint32 isComponentNotDisabled_mb : 1;
    uint32 bits_8_0x1D_0x20 : 0x20 - 0x1D;
    char pad_000C[4]; //0x000C
}; //Size: 0x0010
assert_sizeof(ManagedObject, 0x10);

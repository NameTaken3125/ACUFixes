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
    char pad_0008[8]; //0x0008
}; //Size: 0x0010
assert_sizeof(ManagedObject, 0x10);

#pragma once

#include "basic_types.h"

class TypeInfo
{
public:
    char pad_0000[16]; //0x0000
    void* methods_mb; //0x0010
    char* typeName; //0x0018
    char pad_0020[4]; //0x0020
    __int32 lookupInTypeInfoSystem_mb; //0x0024
    __int32 structSize; //0x0028
    char pad_002C[12]; //0x002C
    TypeInfo* base; //0x0038
    char pad_0040[16]; //0x0040
    void* (__fastcall* Create)(void* createAtAddr); //0x0050
    char pad_0058[56]; //0x0058
    void* Destroy; //0x0090
    void* fn98; //0x0098
    char pad_00A0[4]; //0x00A0
    unsigned char byte_a4; //0x00A4
    char pad_00A5[27]; //0x00A5
}; //Size: 0x00C0
assert_offsetof(TypeInfo, typeName, 0x18);
assert_offsetof(TypeInfo, structSize, 0x28);
assert_offsetof(TypeInfo, Create, 0x50);

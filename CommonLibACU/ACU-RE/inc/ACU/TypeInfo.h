#pragma once

#include "basic_types.h"

#include "ACUHashmap.h"

class DeserializationStream;


class TypeInfo_ClassMember
{
public:
    enum class PrimitiveType : uint32
    {
        TIPT_Bool = 0,
        TIPT_Byte = 3,
        TIPT_Word = 5,
        TIPT_Dword = 7,
        TIPT_Float = 0xA,
        TIPT_Vector3 = 0xD,
        TIPT_Quat = 0xE,
        TIPT_Matrix4 = 0x10,
        TIPT_WeakPtr = 0x12,
        TIPT_RawPtr = 0x14,
        TIPT_Instance = 0x16,
        TIPT_Enum = 0x19,
        TIPT_String = 0x1A,
        TIPT_StrongPtr = 0x1C,
        //TIPT_SmallArray_RawPtr = 0xA1D; // Only appears as the dword_C << 16
        //TIPT_SmallArray_Instance = 0xB1D; // Only appears as the dword_C << 16
        //TIPT_SmallArray_StrongPtr = 0xE1D; // Only appears as the dword_C << 16
    };
    uint32 bits_0_0_0x19 : 0x19;
    uint32 isDeserialized_mb : 1;
    uint32 bits_0_0x1A_0x20 : 0x20 - 0x1A;
    uint32 memberNameHash; //0x0004
    uint32 typeNameHash; //0x0008
    uint32 bits_0xC_0_0x10 : 0x10;
    PrimitiveType primitiveTypeAgainCanAlsoDescribeModifiers : 12;
    PrimitiveType primitiveType : 5;
    uint32 offsetInBitfield : 6;
    uint32 bitWidthInBitfield : 3;
    uint32 bits_0x10_0xB_0x12 : 0x12 - 0xE;
    uint32 offsetInStruct : 0x20 - 0x12;
    char pad_0014[36]; //0x0014
}; //Size: 0x0038
assert_sizeof(TypeInfo_ClassMember, 0x38);

class TypeInfo
{
public:
    using TypeNameHash_t = uint32;

    TypeInfo_ClassMember* members;
    void* enums_mb; //0x0008
    void* methods_mb; //0x0010
    char* typeName; //0x0018
    TypeNameHash_t baseClassHash_mb; //0x0020
    TypeNameHash_t lookupInTypeInfoSystem_mb; //0x0024
    __int32 structSize; //0x0028
    char pad_002C[12]; //0x002C
    TypeInfo* base; //0x0038
    char pad_0040[12]; //0x0040
    uint16 word_4C; //0x004C
    uint16 word_4E; //0x004E
    void* (__fastcall* Create)(void* createAtAddr); //0x0050
    char pad_0058[24]; //0x0058
    void* (__fastcall* DeserializeIfNoVTBL)(void* thisObject, DeserializationStream* deserializationStream); //0x0070
    char pad_0078[24]; //0x0078
    void* Destroy; //0x0090
    void* fn98; //0x0098
    uint16 numMembers; //0x00A0
    char pad_00A2[2]; //0x00A2
    uint8 alignmentIn7lowBits_mb; //0x00A4
    char pad_00A5[11]; //0x00A5
}; //Size: 0x00B0
assert_offsetof(TypeInfo, typeName, 0x18);
assert_offsetof(TypeInfo, structSize, 0x28);
assert_offsetof(TypeInfo, Create, 0x50);
assert_offsetof(TypeInfo, DeserializeIfNoVTBL, 0x70);
assert_sizeof(TypeInfo, 0xB0);

inline bool IsSubclass(TypeInfo& what, TypeInfo& ofWhat)
{
    // Implementation at e.g. 140EA4110
    uint16 v4 = what.word_4E;
    int32 unkdiff = ofWhat.word_4C - what.word_4C;
    return ((unkdiff >> 31) | ~((unkdiff | -unkdiff) >> 31)) & (((v4 - ofWhat.word_4E) >> 31) | ~(((v4 - ofWhat.word_4E) | -(v4 - ofWhat.word_4E)) >> 31)) & 1;
}

namespace ACU::TypeInfos
{
inline ACUHashmap<TypeInfo::TypeNameHash_t, TypeInfo*>* GetTypeInfoSystem()
{
    auto* tisRoot = (ACUHashmap<uint32, TypeInfo*>*)0x14525BA10;
    return tisRoot;
}
inline TypeInfo* FindTypeInfoByTypeNameHash(TypeInfo::TypeNameHash_t typeNameHash)
{
    auto* tisRoot = GetTypeInfoSystem();
    TypeInfo** foundTI = tisRoot->Get(typeNameHash);
    return foundTI ? *foundTI : nullptr;
}
} // namespace ACU::TypeInfos

#pragma once

/*
A huge mess, don't trust the names of classes too much.
*/


#include "ACU/ManagedObject.h"
#include "ACU/SmallArray.h"
#include "ACU/BigArray.h"
#include "ACU/SharedPtr.h"

#include "ACU_DefineNativeFunction.h"


class IReader
{
public:

    // @vtbl == 14363EDC0

    virtual void Unk000_Destroy() = 0;
    virtual char Unk008() = 0;
    virtual void Unk010() = 0;
    virtual bool Unk018_IsEnd() = 0;
    virtual void Unk020() = 0;
    virtual void Unk028_Get_DwordC_Dword10() = 0;
    virtual int  Unk030() = 0;
    virtual int  Unk038() = 0;
    virtual int  Unk040_Read8bytes(uint64* out) = 0;
    virtual int  Unk048_Read4bytes(uint32* out) = 0;
    virtual int  Unk050_Read2bytes(uint16* out) = 0;
    virtual int  Unk058_Read1byte(byte* out) = 0;
    virtual int  Unk060_ReadNBytes(char* out, int numBytes) = 0;
    virtual char Unk068() = 0;
    virtual int  Unk070() = 0;
    virtual char Unk078() = 0;
    virtual char Unk080() = 0;
    virtual char Unk088() = 0;
    virtual char Unk090() = 0;
    virtual char Unk098() = 0;
    virtual void Unk0A0_SeekRelative(int moveHowManyBytes) = 0;
};

class CombinedRawBufsReader;
struct DatapackReader
{
    // @members

    char pad_0000[32]{ 0 };
    uint64 qword_20{ 0 };
    uint64 qword_28{ 0 };
    CombinedRawBufsReader* p30{ 0 };
    uint64 stru_38{ 0 };
    char pad_0040[8]{ 0 };
    uint32 dword_48{ 0 };
    char pad_004C[4]{ 0 };
    uint64 pDecryptedChunk{ 0 };
    uint32 decryptedChunkSize{ 0 };
    uint32 dword_5C{ 0 };
    uint64 nondecryptedChunk{ 0 };
    uint32 decryptedChunkLocalStreampos{ 0 };
    char pad_006C[4]{ 0 };
    uint32 dword_70{ 0 };
    char pad_0074[4]{ 0 };
    uint64 qword_78{ 0 };
    uint32 dword_80{ 0 };
    char pad_0084[4]{ 0 };
    uint8 byte_88{ 0 };
    uint8 byte_89{ 0 };
    char pad_008A[6]{ 0 };

    // @helper_functions

    DatapackReader(CombinedRawBufsReader* p_mergedReader, unsigned __int8 a3, unsigned __int8 a4, char a5, unsigned int a6, unsigned __int8 a7, int a8, char a9, void* p_someAllocator)
    {
        // Implementation at 142747110
        DEFINE_GAME_FUNCTION(
            DatapackReader__ctor
            , 0x142747110, DatapackReader*, __fastcall
            , (DatapackReader * p_datapackReader, CombinedRawBufsReader * p_mergedReader, unsigned __int8 a3, unsigned __int8 a4, char a5, unsigned int a6, unsigned __int8 a7, int a8, char a9, void* p_someAllocator)
        );
        DatapackReader__ctor(this, p_mergedReader, a3, a4, a5, a6, a7, a8, a9, p_someAllocator);
    }
    IReader& CastToReader() { return *(IReader*)this; }
};
assert_sizeof(DatapackReader, 0x90);
class DeserializationStream_PostloadCallback
{
public:
    using Callback_t = void(*)(ManagedObject*, uint32 someDword);

    ManagedObject* receiverManagedObject; //0x0000
    Callback_t callback; //0x0008
    Callback_t secondOptionalCallback; //0x0010
    uint32 dword_18; //0x0018
    char pad_001C[4]; //0x001C
}; //Size: 0x0020
assert_sizeof(DeserializationStream_PostloadCallback, 0x20);
using DeserializationStream_ArrPostloadCallbacks = BigArray<DeserializationStream_PostloadCallback>;
assert_sizeof(DeserializationStream_ArrPostloadCallbacks, 0x10);
class DeserializationCycle;
struct DatapackReader;
class DeserializationStream
{
public:
    char pad_0000[8]; //0x0000
    DeserializationCycle* postLoadCallbacks; //0x0008
    uint64 p10; //0x0010
    void* p18; //0x0018
    void* readStart_from2ndByte; //0x0020
    uintptr_t currentPtr; //0x0028
    uintptr_t readPastEnd; //0x0030
    DatapackReader* p_38; //0x0038
    uint32 dword_40; //0x0040
    char pad_0044[20]; //0x0044
    uint32 dword_58; //0x0058
    char pad_005C[4]; //0x005C
    CombinedRawBufsReader* combinedRawBufsReader; //0x0060
    DatapackReader* datapackReader; //0x0068
    char pad_0070[5]; //0x0070
    char byte_75; //0x0075
    char pad_0076[2]; //0x0076
    uint64 handle_FirstInData; //0x0078

    // @helper_functions
    template<typename POD>
    void ReadRawBytes(POD& out)
    {
        std::memcpy(&out, (const void*)currentPtr, sizeof(POD));
        currentPtr += sizeof(POD);
    }
    static void ReadMore_mb(DeserializationStream* this_)
    {
        using DeserializationStream__ReadMore_mb_fnt = void(__fastcall*)(DeserializationStream*);
        ((DeserializationStream__ReadMore_mb_fnt)0x142705370)(this_);
    }
}; //Size: 0x0080
assert_offsetof(DeserializationStream, datapackReader, 0x68);
class DatapackPackedObjectDesc
{
public:
    uint64 handle; //0x0000
    SmallArray<uint16> arr8; //0x0008
    char pad_0014[8]; //0x0014
    uint8 byte_1C; //0x001C
    char pad_001D[3]; //0x001D
    uint32 filesizePlusCplusLenFilename; //0x0020
    char pad_0024[20]; //0x0024
}; //Size: 0x0038
assert_sizeof(DatapackPackedObjectDesc, 0x38);
class UsedDuringLoadDataPack_228_RequestedDatapackResolvedPrefetches;
class DatapackDescriptorMore;
class DeserializationCycle
{
public:
    // @members

    char pad_0000[64]; //0x0000
    BigArray<DatapackPackedObjectDesc> objectsInCurrentRelatedDatapack; //0x0040
    char pad_0054[12]; //0x0054
    BigArray<SharedPtrNew<ManagedObject>*> objectsSharedPtrsFromAllPrefetchedDatapacks; //0x0060
    void* p70; //0x0070
    char pad_0078[8]; //0x0078
    std::array<DeserializationStream_ArrPostloadCallbacks, 16> callbackStages_mb; //0x0080
    char pad_0180[32]; //0x0180
    DeserializationStream deserStream; //0x01A0
    uint64 currentHandle_mb; //0x0220
    UsedDuringLoadDataPack_228_RequestedDatapackResolvedPrefetches* p228; //0x0228
    DatapackDescriptorMore* p230; //0x0230
    char pad_0238[72]; //0x0238

    // @helper_functions

    DeserializationCycle() {
        DEFINE_GAME_FUNCTION(DeserializationCycle__ctor, 0x1426E0B50, void, __fastcall, (DeserializationCycle * a1, __int64 a2));
        DeserializationCycle__ctor(this, 0);
    }
}; //Size: 0x240 at least
assert_offsetof(DeserializationCycle, callbackStages_mb, 0x80);
assert_offsetof(DeserializationCycle, deserStream, 0x1A0);

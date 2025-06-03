#pragma once

#include "SmallArray.h"
#include "BigArray.h"
#include "ACUHashmap.h"

class DatapackDescriptor
{
public:
    uint64 offsetInForgeFile; //0x0000
    uint64 handleFirstInDatapack; //0x0008
    uint32 packedSize; //0x0010
    char pad_0014[4]; //0x0014
}; //Size: 0x0018
assert_sizeof(DatapackDescriptor, 0x18);

class ForgeFile_240;

class ForgeFile_38_40
{
public:
    uint32 numFiles; //0x0000
    char pad_0004[4]; //0x0004
    uint64 streampos_datapackDescriptionsSmall; //0x0008
    uint64 streampos_datapackDescriptionsBig; //0x0010
    uint64 streampos_afterDatapackDescriptionsBigBeforeLostAndFound; //0x0018
    char pad_0020[8]; //0x0020
}; //Size: 0x0028
assert_sizeof(ForgeFile_38_40, 0x28);

class ForgeFile_38
{
public:
    // @members
    char pad_0000[8]; //0x0000
    uint64 qword_8; //0x0008
    uint32 int_10; //0x0010
    uint32 int_14; //0x0014
    uint32 int_18; //0x0018
    char pad_001C[20]; //0x001C
    ForgeFile_240* p30; //0x0030
    uint64 qword_38; //0x0038
    BigArray<ForgeFile_38_40> bigArr_40; //0x0040
    BigArray<DatapackDescriptor> arrDatapackDescs; //0x0050
    ACUHashmap<uint64, int> hashmap_datapackIdxByFirstHandle; //0x0060
    uint64 qword_68; //0x0068
    char pad_0070[8]; //0x0070

    // @helper_functions
    ForgeFile_38()
     : qword_8(0)
     , int_10(-1)
     , int_14(-1)
     , int_18(-1)
     , p30(nullptr)
     , qword_38(0)
     , qword_68(0)
    {}
}; //Size: 0x0078
assert_sizeof(ForgeFile_38, 0x78);

class ForgeFile_240
{
public:
    char pad_0000[32]; //0x0000
    uint64 openFileHandle; //0x0020
    char pad_0028[8]; //0x0028
    void* bigArr30; //0x0030
    char pad_0038[24]; //0x0038
}; //Size: 0x0050

class ForgeFile_DatapackPrefetchInfo
{
public:
    // @members
    uint64 firstHandle; //0x0000
    uint32 prefetchDataSize : 31;
    uint32 isCompressed_mb : 1;
    uint32 offsetInPrefetchData; //0x000C
}; //Size: 0x0010
assert_sizeof(ForgeFile_DatapackPrefetchInfo, 0x10);

class ForgeFile_2A0;
class ForgeFile_Header
{
public:
    char str_scimitar[9]; //0x0000
    char pad_0009[3]; //0x0009
    uint32 dword_C_eq0x1b; //0x000C
    uint64 qword_10_eq0x41A; //0x0010
    uint64 metadataPackHandle; //0x0018
    uint8 byte_20; //0x0020
    char pad_0021[7]; //0x0021
}; //Size: 0x0028

class ForgeFile
{
public:
    // @members
    char pad_0000[8]; //0x0000
    uint32 someLock; //0x0008
    char pad_000C[4]; //0x000C
    ForgeFile_Header header; //0x0010
    ForgeFile_38* p38; //0x0038
    char filename[256]; //0x0040
    char pad_0140[256]; //0x0140
    ForgeFile_240 stru_240; //0x0240
    char pad_0290[16]; //0x0290
    ForgeFile_2A0* p2A0; //0x02A0
    char pad_02A8[16]; //0x02A8
    char* dirpathIfNotRootFolder_mb; //0x02B8
    uint8 byte_2C0; //0x02C0
    char pad_02C1[51]; //0x02C1
    uint32 filenameHash; //0x02F4
    uint32 someNull_2F8; //0x02F8
    int32 someHandle; //0x02FC
    char pad_0300[24]; //0x0300
    uint64 prefetchInfoHandle; //0x0318
    char pad_0320[16]; //0x0320
    byte* prefetchData; //0x0330
    BigArray<ForgeFile_DatapackPrefetchInfo> datapacksPrefetchInfoAscendingHandles; //0x0338

    // @helper_functions
    ForgeFile();
}; //Size: 0x0348
assert_sizeof(ForgeFile, 0x348);

using ForgeIndex_t = uint32;

class ForgeFileEntry
{
public:
    // @members
    char* forgeName; //0x0000
    ForgeIndex_t forgeIdx_mb; //0x0008
    char pad_000C[4]; //0x000C
    ForgeFile* forgeContentsDescriptor; //0x0010
    uint64 qword_18; //0x0018
    uint64 qword_20; //0x0020
    uint32 dword_28; //0x0028
    uint32 refcount_mb; //0x002C
    SmallArray<ForgeIndex_t> forgeIndicesOfDependentForges; //0x0030
    uint8 byte_3C; //0x003C
    char pad_003D[3]; //0x003D

    // @helper_functions

    // See constructor at 142737A6C
    ForgeFileEntry()
     : forgeName(nullptr)
     , forgeContentsDescriptor(nullptr)
     , forgeIdx_mb(-1)
     , qword_18(0)
     , qword_20(0)
     , dword_28(0)
     , refcount_mb(1)
     , byte_3C(0)
    {}
}; //Size: 0x0040
assert_sizeof(ForgeFileEntry, 0x40);

class ForgeManager
{
public:
    // @vtbl
    virtual void Unk000();
    virtual void Unk008();
    virtual void Unk010();
    virtual void Unk018();
    virtual void Unk020();
    virtual void Unk028();
    virtual void Unk030();
    virtual void Unk038();
    virtual void Unk040();
    virtual void Unk048();
    virtual void Unk050();
    virtual void Unk058();
    virtual void Unk060();
    virtual void Unk068();
    virtual void Unk070();
    virtual void Unk078();
    virtual void Unk080();
    virtual void Unk088();
    virtual void Unk090();
    virtual void Unk098();
    virtual void Unk0A0();
    virtual void Unk0A8();
    virtual void Unk0B0();
    virtual void Unk0B8();
    virtual void Unk0C0();
    virtual void Unk0C8();
    virtual void Unk0D0();
    virtual void Unk0D8();
    virtual void Unk0E0();
    virtual void Unk0E8();
    virtual void Unk0F0();
    virtual void Unk0F8();
    virtual void Unk100();
    virtual void Unk108();
    virtual void Unk110();
    virtual void Unk118();
    virtual void Unk120();
    virtual void Unk128();
    virtual void Unk130();
    virtual void Unk138();
    virtual void Unk140();
    virtual void Unk148();
    virtual void Unk150();
    virtual void Unk158();
    virtual void Unk160_GetDatapackSizeAndOffset(unsigned __int64 p_handleFirstInDatapack, uint32* p_forgeIdxOut, ForgeFile** p_forgeDescriptorOut, __int64 p_offsetInForgeFileOut, __int64 p_sizePackedOut, SmallArray<uint32>* p_arrForgeIndicesOut); // 142733330

    // @members
    char pad_0008[184]; //0x0008
    ForgeIndex_t nextForgeIdx; //0x00C0
    SmallArray<ForgeFileEntry*> forges; //0x00C4
    uint64 criticalSection_forgeEntries; //0x00D0
    char pad_00D8[60]; //0x00D8
    uint8 isNewEntriesNeedToOpen; //0x0114
    char pad_0115[3]; //0x0115

    // @helper_functions
    static ForgeManager* GetSingleton() { return *(ForgeManager**)0x14525BDB0; }
    bool IsForgeAlive(ForgeIndex_t forgeIdx)
    {
        return std::find_if(forges.begin(), forges.end(), [&](ForgeFileEntry* forgeEntry)
            {
                return forgeEntry->forgeIdx_mb == forgeIdx;
            }) != forges.end();
    }
    ForgeFileEntry* FindForgeForIdx(ForgeIndex_t forgeIdx)
    {
        for (ForgeFileEntry* forgeEntry : forges)
        {
            if (forgeEntry->forgeIdx_mb == forgeIdx) return forgeEntry;
        }
        return nullptr;
    }
    void DecrementForgeRefcount(ForgeIndex_t forgeIdx)
    {
        auto* ForgeManager__DecrementForgeEntryRefcount_mb = (void (*__fastcall)(ForgeManager* a1, ForgeIndex_t p_forgeIdx))0x142721FE0;
        ForgeManager__DecrementForgeEntryRefcount_mb(this, forgeIdx);
    }
}; //Size: 0x0118
assert_sizeof(ForgeManager, 0x118);

#include "pch.h"

#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"
#include "ACU/BigArray.h"
#include "ACU_DefineNativeFunction.h"
#include "Common_Plugins/Common_PluginSide.h"

#include "Handles.h"

#include "MyLog.h"
static DEFINE_LOGGER_CONSOLE_AND_FILE(VirtualForgesLog, "[AssetOverrides]");

class Mock_IReader
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
};
class CombinedRawBufsReader;
struct UsedDuringLoadDataPackGetLinkedObjects
{
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

    UsedDuringLoadDataPackGetLinkedObjects(CombinedRawBufsReader* p_mergedReader, unsigned __int8 a3, unsigned __int8 a4, char a5, unsigned int a6, unsigned __int8 a7, int a8, char a9, void* p_someAllocator);
};
// Implementation at 142747110
UsedDuringLoadDataPackGetLinkedObjects::UsedDuringLoadDataPackGetLinkedObjects(CombinedRawBufsReader* p_mergedReader, unsigned __int8 a3, unsigned __int8 a4, char a5, unsigned int a6, unsigned __int8 a7, int a8, char a9, void* p_someAllocator)
{
    DEFINE_GAME_FUNCTION(
        UsedDuringLoadDataPackGetLinkedObjects__ctor
        , 0x142747110, UsedDuringLoadDataPackGetLinkedObjects*, __fastcall
        , (UsedDuringLoadDataPackGetLinkedObjects* p_datapackParser, CombinedRawBufsReader* p_mergedReader, unsigned __int8 a3, unsigned __int8 a4, char a5, unsigned int a6, unsigned __int8 a7, int a8, char a9, void* p_someAllocator)
    );
    UsedDuringLoadDataPackGetLinkedObjects__ctor(this, p_mergedReader, a3, a4, a5, a6, a7, a8, a9, p_someAllocator);
}
assert_sizeof(UsedDuringLoadDataPackGetLinkedObjects, 0x90);

class Mock_CombinedRawBufsReader : public Mock_IReader
{
public:
    virtual void Unk000_Destroy() override {}
    virtual char Unk008() override { return 1; }
    virtual void Unk010() override {}
    virtual bool Unk018_IsEnd() override { return streampos >= packedSize; }
    virtual void Unk020() override {}
    virtual void Unk028_Get_DwordC_Dword10() override {}
    virtual int  Unk030() override { return 0; }
    virtual int  Unk038() override { return 0; }
    virtual int  Unk040_Read8bytes(uint64* out) override { return Unk060_ReadNBytes((char*)out, 8); }
    virtual int  Unk048_Read4bytes(uint32* out) override { return Unk060_ReadNBytes((char*)out, 4); }
    virtual int  Unk050_Read2bytes(uint16* out) override { return Unk060_ReadNBytes((char*)out, 2); }
    virtual int  Unk058_Read1byte(byte* out) override { return Unk060_ReadNBytes((char*)out, 1); }
    virtual int  Unk060_ReadNBytes(char* out, int numBytes) override;
    virtual char Unk068() { return 0; }
    virtual int  Unk070() { return 0; }
    virtual char Unk078() { return 1; }
    virtual char Unk080() { return 1; }
    virtual char Unk088() { return 1; }
    virtual char Unk090() { return 1; }
    virtual char Unk098() { return 1; }
    virtual void Unk0A0_MovePos_mb(int moveHowManyBytes);
    virtual void Unk0A8() {}
    virtual int  Unk0B0() { return 0; }
    virtual void Unk0B8() {}
    virtual void Unk0C0() {}
    virtual int  Unk0C8() { return 0; }
    virtual void Unk0D0() {}
    virtual void Unk0D8() {}
    virtual void Unk0E0() {}
    virtual void Unk0E8() {}
    virtual void Unk0F0_Clear_mb() {}
    virtual char Unk0F8() { return (byte_28 >> 1) & 1; }
    virtual void Unk100() {}

    // @members // See initialization at around 142756F03
    uint32 dword_8{ 0 }; //0x0008
    uint32 dword_C{ 0 }; //0x000C
    uint32 dword_10{ 0 }; //0x0010
    char pad_0014[4]{ 0 }; //0x0014
    uint64 qword_18{ 0 }; //0x0018
    uint64 handleFirstInDataPack{ 0 }; //0x0020
    uint8 byte_28{ 0 }; //0x0028
    char pad_0029[39]{ 0 }; //0x0029
    uint64 qword_50{ 0 }; //0x0050
    char pad_0058[16]{ 0 }; //0x0058
    uint32 dword_68{ 0 }; //0x0068
    char pad_006C[64]{ 0 }; //0x006C
    uint32 streampos{ 0 }; //0x00AC
    uint32 packedSize{ 0 }; //0x00B0
    char pad_00B4[4]{ 0 }; //0x00B4
    BigArray<void*> arrB8_mb; //0x00B8

    // Additional members for the mock reader
    std::ifstream m_File;

    void Init_2(uint32 packedSize_);
};


// Implementation from 142764E30
void Mock_CombinedRawBufsReader::Init_2(uint32 packedSize_)
{
    byte_28 |= 3u;
    const uint64 handleFirstInDatapack_UNKNOWN = 0;
    handleFirstInDataPack = handleFirstInDatapack_UNKNOWN;
    streampos = 0;
    packedSize = packedSize_;
}
int Mock_CombinedRawBufsReader::Unk060_ReadNBytes(char* out, int numBytes)
{
    m_File.read(out, numBytes);
    return 1;
}
void Mock_CombinedRawBufsReader::Unk0A0_MovePos_mb(int moveHowManyBytes)
{
    m_File.seekg(moveHowManyBytes, std::ios_base::cur);
}
class DatapackPackedObjectDesc
{
public:
    uint64 handle; //0x0000
    SmallArraySemistatic<uint16, 4> arr8; //0x0008
    uint8 byte_1C; //0x001C
    char pad_001D[3]; //0x001D
    uint32 filesizePlusCplusFilename; //0x0020
    char pad_0024[20]; //0x0024
}; //Size: 0x0038
assert_sizeof(DatapackPackedObjectDesc, 0x38);
DEFINE_GAME_FUNCTION(LinkedObjectDesc__Fill, 0x1426EBF50, int, __fastcall, (DatapackPackedObjectDesc* p_linkedObjInfo, UsedDuringLoadDataPackGetLinkedObjects* p_datapackParser));
// WARNING: No error checking. It cannot even handle an empty file - just hangs forever.
std::optional<uint64> FindFirstHandleInDatapack(const fs::path& targetFilepath)
{
    /*
    Partially recreating a chain of actions that occur during the call to 142701730
    in order to retrieve the handles within the datapack.
    The recreated portion of logic is:
    within 142701730:
        14270174C calls 142764E30,      (initiate compressed file reader)
        then within 142704CE0:
            142704D44 calls 142747110   (initiate "gather information about datapack contents")
            142704D7A calls 1427665D0   (read 2 bytes == number of files within datapack)
            142704DBF calls 1426EBF50   (read info about next file in datapack)
    */
    std::ifstream infile = std::ifstream(targetFilepath, std::ios::binary);
    if (!infile) return {};
    infile.seekg(0, std::ios::end);
    uint32 filesize = (uint32)infile.tellg();
    infile.seekg(0);


    fs::path parentPath = targetFilepath.parent_path().filename();
    Mock_CombinedRawBufsReader reader;
    reader.m_File = std::move(infile);
    reader.Init_2(filesize);


    void* someAllocatorUsedDuringLoadDatapack = *(void**)0x1452585E0;
    void* theMoreCommonAllocator = *(void**)0x1452585D0;
    // Constructor called at 142704D44.
    UsedDuringLoadDataPackGetLinkedObjects retrieveDatapackStats(
        (CombinedRawBufsReader*)&reader,
        0, 1u, 0, 0x8000u, 0, 10, 0, theMoreCommonAllocator
    );

    // The game does not check for errors, so attempting to retrieve the handle from
    // an invalid datapack (e.g. just an empty file) will freeze the game.
    // I need to discern ahead of time the datapacks that will cause a freeze.
    // This criterium below seems to be working alright, though I don't know what it means.
    const bool isLooksLikeValidDatapack = retrieveDatapackStats.byte_89 == 9;
    if (!isLooksLikeValidDatapack)
    {
        LOG_DEBUG(VirtualForgesLog
            , "[error]\"%s\" does not look like a valid datapack.\n"
              "    The game will not do any error checking and can freeze, so I'm aborting now.\n"
            , targetFilepath.filename().u8string().c_str()
        );
        return {};
    }

    uint16 numFilesInDatapack = 0;
    using IReader = Mock_IReader;
    ((IReader&)retrieveDatapackStats).Unk050_Read2bytes(&numFilesInDatapack);

    if (!numFilesInDatapack) return {};
    DatapackPackedObjectDesc firstFileDesc;
    LinkedObjectDesc__Fill(&firstFileDesc, &retrieveDatapackStats);
    return firstFileDesc.handle;
    //std::vector<DatapackPackedObjectDesc> datapackContents;
    //datapackContents.resize(numFilesInDatapack);
    //for (uint16 i = 0; i < numFilesInDatapack; i++)
    //{
    //    LinkedObjectDesc__Fill(&datapackContents[i], &retrieveDatapackStats);
    //    LOG_DEBUG(VirtualForgesLog
    //        , L"[LoadDatapack]%3hu. %llu => %s"
    //        , i
    //        , datapackContents[i].handle
    //        , utf8_and_wide_string_conversion::utf8_decode(ACU::Handles::HandleToText(datapackContents[i].handle).c_str()).c_str()
    //    );
    //}
    //return datapackContents[0].handle;
}

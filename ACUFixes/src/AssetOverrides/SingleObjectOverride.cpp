#include "pch.h"

#include "ACU/ManagedObject.h"
#include "ACU/BigArray.h"
#include "ACU/Memory/ACUAllocs.h"
#include "ACU/AtomGraph.h"
#include "ACU/AtomRTCPDescriptor.h"
#include "ACU/ManagedPtrs/ManagedPtrs.h"

#include "ACU_DefineNativeFunction.h"

#include "MyLog.h"
static DEFINE_LOGGER_CONSOLE_AND_FILE(SOOLog, "[SOOverride]");

fs::path AbsolutePathInThisDLLDirectory(const fs::path& filenameRel);

#include "AssetOverrides.h"

#include "DeserializationCycle.h"

DEFINE_GAME_FUNCTION(DeserializationStream__CreateObjectForTypehashAndHandle, 0x1426EB3B0, ManagedObject*, __fastcall, (DeserializationStream* a1, unsigned int p_hashInTypeInfosSystem, unsigned __int64 p_handle));

DEFINE_GAME_FUNCTION(JoinManagedObjectAndHandle_mb, 0x142714230, void, __fastcall, (__int64 p_handle, ManagedObject* p_manObj));
DEFINE_GAME_FUNCTION(DeserializationStream__From2ndByte, 0x14270B510, void, __fastcall, (DeserializationStream* a1, ManagedObject** a2));

static uint32 ConsumeFilename(DatapackReader* a1, uint32* p_firstDwordOut, uint32* p_secondDwordOut)
{
    a1->CastToReader().Unk048_Read4bytes(p_firstDwordOut);
    a1->CastToReader().Unk048_Read4bytes(p_secondDwordOut);
    uint32 lenFilename;
    a1->CastToReader().Unk048_Read4bytes(&lenFilename);
    a1->CastToReader().Unk0A0_SeekRelative(lenFilename);
    uint32 numConsumedBytes = 0xC + lenFilename;
    return numConsumedBytes;
}
static size_t ConsumePrologue(DatapackReader* a1, __int64 a2, uint32 a3, __int64 a6)
{
    void* v7 = alloca(0x20E0);
    byte isHasPrerequisitesSection;
    a1->CastToReader().Unk058_Read1byte(&isHasPrerequisitesSection);
    if (isHasPrerequisitesSection)
    {
        *(byte*)(a6 + 0x438) = 1;
        DEFINE_GAME_FUNCTION(ParsePrerequisitesBeforeDeserialize_ctor, 0x142746FA0, void, __fastcall, (void* a1));
        DEFINE_GAME_FUNCTION(sub_14276E780, 0x14276E780, void, __fastcall, (void* a1, DatapackReader * a2, char a3));
        DEFINE_GAME_FUNCTION(ParsePrerequisitesBeforeDeserialize__ConsumePrerequisites, 0x1427081B0, void, __fastcall, (__int64 a1, void* rdx0));
        DEFINE_GAME_FUNCTION(sub_142749E60, 0x142749E60, void, __fastcall, (void* a1));
        ParsePrerequisitesBeforeDeserialize_ctor(v7);
        sub_14276E780(v7, a1, 0);
        ParsePrerequisitesBeforeDeserialize__ConsumePrerequisites(a6, v7);
        sub_142749E60(v7);
    }
    else
    {
        *(byte*)(a6 + 0x438) = 0;
    }
    // Usually the "isHasPrerequisitesSection" byte is 0, so the total consumed is 1 byte.
    // In the case of Animation files, "isHasPrerequisitesSection" is 1, and the 4 function calls above
    // then actually do the work of consuming the "prerequisites section".
    // As far as I can tell, the number of consumed bytes isn't stored anywhere,
    // so I'd need to either pass a mock DatapackReader (and there are too many virtual functions
    // I don't know the meaning of) or to replicate the logic of the function call 0x1427081B0,
    // which seems like a fairly straightforward series of primitive reads.
    // Regardless, right now I'm only supporting the case of 1-byte prologue.
    // This is enough for Material, Mesh, TextureMap, AtomGraph, SoftBodySettings, Cloth...
    size_t numConsumedBytesInPrologue = 1;
    return numConsumedBytesInPrologue;
}

class SomeMissingLoggerMaybe
{
public:
    virtual void Unk000();
    virtual void Unk008();
    virtual void Unk010();
    virtual void Unk018(__int64, __int64, __int64);

    static SomeMissingLoggerMaybe* GetSingleton() { return *(SomeMissingLoggerMaybe**)0x1452585A0; }
};
std::vector<byte> GetSingleObjectOverrideBytes(uint64 handle);
void WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement(uint64 rax, uint64 rbx, uint64 rsi, uint64 rbp, uint64 rsp, uint64 r13)
{
    DEFINE_GAME_FUNCTION(DatapackParser__SkipPrologueOfFileInDatapack, 0x1426EBBF0, void, __fastcall, (DatapackReader * a1, __int64 a2, uint32 a3, uint32 * p_firstDwordOut, uint32 * p_secondDwordOut, __int64 a6));
    auto* deserCycle = (DeserializationCycle*)rbx;

    auto* currentPackedObjectDesc = (DatapackPackedObjectDesc*)(rsi - 0x20);
    uint32 originalObjectUnpackedSizePlusPrologue = currentPackedObjectDesc->filesizePlusCplusLenFilename;
    uint64 handle = currentPackedObjectDesc->handle;

    auto* datapackReader = deserCycle->deserStream.datapackReader;
    std::vector<byte> modFileAsBytes = GetSingleObjectOverrideBytes(handle);
    if (modFileAsBytes.empty())
    {
        // normal handling...
        ConsumeFilename(
            datapackReader,
            (uint32*)(rsp + 0x54),
            (uint32*)(rsp + 0x50));
        ConsumePrologue(
            datapackReader,
            0,
            (uint32&)rax,
            rbp + 0x1D8
        );

        void* v37 = (void*)(rbp + 0x1D8);
        if (!*(char*)(rbp + 0x610))
            v37 = 0;
        deserCycle->deserStream.p18 = v37;

        if (*(uint64*)0x1452585A0)
        {
            //v38 = v1->numObjectsInDatapack;
            //v39 = *(_QWORD*)qword_1452585A0;
            //isLocalCache = *(_DWORD*)filesizePlusCplusFilename >> 31;
            //v41 = *(_DWORD*)filesizePlusCplusFilename & 0x7FFFFFFF;
            //v42 = sub_14275A7F0();
            //bigfileOrLocalcache = (const __int64*)"Bigfile";
            //if (isLocalCache & 1)
            //    bigfileOrLocalcache = "LocalCache";
            //v44 = stringFormat_mb("[%s] [%dK] %s", (__int64)bigfileOrLocalcache, (unsigned int)v41 >> 10, v42);
            //(*(void(__fastcall**)(__int64, __int64, _QWORD, _QWORD))(v39 + 24))(qword_1452585A0, v44, vars28, v38);
            //v23 = v75;
            //filesizePlusCplusFilename = handleFirstInData_1;
        }
        uint64 v24 = r13;
        *(uint64*)(rsp + 0x70) = v24;
        deserCycle->deserStream.byte_75 = v24 != 0;
        deserCycle->deserStream.p_38 = datapackReader;
        deserCycle->deserStream.dword_40 = *(uint32*)(rsp + 0x50);
        deserCycle->deserStream.dword_58 = *(uint32*)(rsp + 0x50);

        DeserializationStream__From2ndByte(&deserCycle->deserStream, (ManagedObject**)(rsp + 0x70));
        return;
    }
    uint32 numConsumedBytes = ConsumeFilename(
        datapackReader,
        (uint32*)(rsp + 0x54),
        (uint32*)(rsp + 0x50));
    datapackReader->CastToReader().Unk0A0_SeekRelative(originalObjectUnpackedSizePlusPrologue - numConsumedBytes);

    //LOG_DEBUG(SOOLog,
    //    "  filename: %u bytes, byte_89: %u, 1stDword: %x, 2ndDword: %x\n"
    //    , numConsumedBytes - 0xC
    //    , (uint32)datapackReader->byte_89
    //    , *(uint32*)(rsp + 0x54)
    //    , *(uint32*)(rsp + 0x50)
    //);
    struct
    {
        uint64 pDecryptedChunk;
        uint32 decryptedChunkLocalStreampos;
        uint32 decryptedChunkSize;
        byte byte_89;
    } mockDecryptedStreamState{
            (uint64)&modFileAsBytes[0],
            (uint32)0,
            (uint32)modFileAsBytes.size(),
            (byte)1
    };
    std::swap(datapackReader->pDecryptedChunk, mockDecryptedStreamState.pDecryptedChunk);
    std::swap(datapackReader->decryptedChunkLocalStreampos, mockDecryptedStreamState.decryptedChunkLocalStreampos);
    std::swap(datapackReader->decryptedChunkSize, mockDecryptedStreamState.decryptedChunkSize);
    std::swap(datapackReader->byte_89, mockDecryptedStreamState.byte_89);
    size_t numConsumedBytesInPrologue = ConsumePrologue(
        datapackReader,
        0,
        (uint32&)rax,
        rbp + 0x1D8
    );
    numConsumedBytesInPrologue = datapackReader->decryptedChunkLocalStreampos;
    std::swap(datapackReader->pDecryptedChunk, mockDecryptedStreamState.pDecryptedChunk);
    std::swap(datapackReader->decryptedChunkLocalStreampos, mockDecryptedStreamState.decryptedChunkLocalStreampos);
    std::swap(datapackReader->decryptedChunkSize, mockDecryptedStreamState.decryptedChunkSize);
    std::swap(datapackReader->byte_89, mockDecryptedStreamState.byte_89);
    void* v37 = (void*)(rbp + 0x1D8);
    if (!*(char*)(rbp + 0x610))
        v37 = 0;
    deserCycle->deserStream.p18 = v37;
    if (auto* someLog = SomeMissingLoggerMaybe::GetSingleton())
    {
        // Never seen this section reached.
        // Never seen this global being non-NULL.
        bool isLocalCache = (currentPackedObjectDesc->filesizePlusCplusLenFilename >> 31) & 1;
        uint32 v41 = currentPackedObjectDesc->filesizePlusCplusLenFilename & 0x7FFFFFFF;
        DEFINE_GAME_FUNCTION(sub_14275A7F0, 0x14275A7F0, __int64, __fastcall, ());
        __int64 v42 = sub_14275A7F0();
        const char* bigfileOrLocalcache = "Bigfile";
        if (isLocalCache)
            bigfileOrLocalcache = "LocalCache";
        //__int64 v44 = stringFormat_mb("[%s] [%dK] %s", (__int64)bigfileOrLocalcache, (unsigned int)v41 >> 10, v42);
        //someLog->Unk018(v44, *(uint32*)(rbp + 0x688), deserCycle->objectsInCurrentRelatedDatapack.size);
        //r15 = *(uint64*)(rbp - 0x78);
        //rsi = *(uint64*)(rsp + 0x58);
    }
    uint64 v24 = r13;
    *(uint64*)(rsp + 0x70) = v24;
    deserCycle->deserStream.byte_75 = v24 != 0;
    deserCycle->deserStream.p_38 = datapackReader;
    deserCycle->deserStream.dword_40 = *(uint32*)(rsp + 0x50);
    deserCycle->deserStream.dword_58 = *(uint32*)(rsp + 0x50);

    struct
    {
        uint64 currentPtr;
        uint64 readPastEnd;
    } mockDeserStreamState{
            (uint64)&modFileAsBytes[numConsumedBytesInPrologue],
            (uint64)&modFileAsBytes[0] + modFileAsBytes.size(),
    };
    std::swap(deserCycle->deserStream.currentPtr, mockDeserStreamState.currentPtr);
    std::swap(deserCycle->deserStream.readPastEnd, mockDeserStreamState.readPastEnd);
    DeserializationStream__From2ndByte(&deserCycle->deserStream, (ManagedObject**)(rsp + 0x70));
    std::swap(deserCycle->deserStream.currentPtr, mockDeserStreamState.currentPtr);
    std::swap(deserCycle->deserStream.readPastEnd, mockDeserStreamState.readPastEnd);
}














#include "SingleObjectOverride.h"
std::optional<ScannedLooseFile> ScanGameObjectLooseFile(const fs::path& targetFilepath)
{
    std::ifstream infile = std::ifstream(targetFilepath, std::ios::binary);
    if (!infile) return {};
    infile.seekg(0, std::ios::end);
    uint32 filesize = (uint32)infile.tellg();
    if (filesize <= 14) return {};
    // The game isn't going to check for malformed files,
    // so I'll use some simple criteriums for initial check:
    // I expect the first two bytes to be [0, 1] (is the case
    // for all loose files I checked except Animation) and I also try to look up the TypeInfo
    // by the typename hash (bytes [10:14]).
    byte isHasPrerequisites; infile.seekg(0); infile.read((char*)&isHasPrerequisites, sizeof(isHasPrerequisites));
    std::streampos endOfPrerequisitesSection;
    auto ScanFromFirstByteAfterPrerequisitesSection = [&]() -> std::optional<ScannedLooseFile>
        {
            byte byte1;             infile.read((char*)&byte1, sizeof(byte1));
            uint64 handle;          infile.read((char*)&handle, sizeof(handle));
            uint32 typeNameHash;    infile.read((char*)&typeNameHash, sizeof(typeNameHash));
            if (byte1 != 1) return {};
            TypeInfo* foundTI = ACU::TypeInfos::FindTypeInfoByTypeNameHash(typeNameHash);
            if (!foundTI) return {};
            return ScannedLooseFile{ handle, typeNameHash, *foundTI };
        };
    if (!isHasPrerequisites)
    {
        return ScanFromFirstByteAfterPrerequisitesSection();
    }
    // Replicating the logic at 1427081B0.
    uint16 Version; infile.read((char*)&Version, sizeof(Version));
    if (Version == 2) {
        byte PartialSerialization;  infile.read((char*)&PartialSerialization, sizeof(PartialSerialization));
        uint32 numPrereqs;          infile.read((char*)&numPrereqs, sizeof(numPrereqs));
        for (uint32 i = 0; i < numPrereqs; i++) {
            uint64 Type;            infile.read((char*)&Type, sizeof(Type));
            uint32 Size;            infile.read((char*)&Size, sizeof(Size));
        }
        if (PartialSerialization) {
            uint32 PartialSerializationSize; infile.read((char*)&PartialSerializationSize, sizeof(PartialSerializationSize));
            for (uint32 i = 0; i < PartialSerializationSize; i++) {
                uint64 Type;        infile.read((char*)&Type, sizeof(Type));
                uint32 Size;        infile.read((char*)&Size, sizeof(Size));
            }
        }
    }
    else if (Version == 1) {
        uint32 numPrereqs;          infile.read((char*)&numPrereqs, sizeof(numPrereqs));
        for (uint32 i = 0; i < numPrereqs; i++) {
            uint64 Type;            infile.read((char*)&Type, sizeof(Type));
            uint32 Size;            infile.read((char*)&Size, sizeof(Size));
        }
    }
    else if (!Version) {
        uint32 numPrereqs;          infile.read((char*)&numPrereqs, sizeof(numPrereqs));
        for (uint32 i = 0; i < numPrereqs; i++) {
            uint32 ClassID;         infile.read((char*)&ClassID, sizeof(ClassID));
            uint32 Size;            infile.read((char*)&Size, sizeof(Size));
        }
    }
    uint32 endOfPrereqs = infile.tellg();
    auto result = ScanFromFirstByteAfterPrerequisitesSection();
    if (!result) return {};
    //LOG_DEBUG(SOOLog,
    //    "Prerequisites parsed. Version: %hd, Good: %d, EoP: %3x, H:%12llu, type: %12s, file: %s"
    //    , Version
    //    , (int)infile.good()
    //    , endOfPrereqs
    //    , result->m_Handle
    //    , result->m_CorrespondingTypeInfo.get().typeName
    //    , targetFilepath.u8string().c_str()
    //);
    return result;
}

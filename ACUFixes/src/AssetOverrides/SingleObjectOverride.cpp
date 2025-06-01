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

#include "DeserializationCycle.h"

DEFINE_GAME_FUNCTION(DeserializationStream__CreateObjectForTypehashAndHandle, 0x1426EB3B0, ManagedObject*, __fastcall, (DeserializationStream* a1, unsigned int p_hashInTypeInfosSystem, unsigned __int64 p_handle));

// Thanks, https://stackoverflow.com/questions/15138353/how-to-read-a-binary-file-into-a-vector-of-unsigned-chars/21802936#21802936
static std::vector<byte> readFile(const fs::path& filename)
{
    // open the file:
    std::ifstream file(filename, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<byte> vec;
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
        std::istream_iterator<byte>(file),
        std::istream_iterator<byte>());

    return vec;
}
struct MockDeserializationStream
{
    DeserializationStream deserStream;
    DeserializationCycle deserStream_8;
    std::vector<byte> fileAsBytes;

    fs::path m_Filepath;
    byte m_1stByte; // == 0
    byte m_2ndByte; // == 1
    uint64 m_Handle;
    uint32 m_TypeNameHash;

    MockDeserializationStream(const fs::path& filepath)
        : m_Filepath(filepath)
        , deserStream{ 0 }
        , deserStream_8{}
        , fileAsBytes(readFile(filepath))
    {
        m_1stByte = (byte&)fileAsBytes[0];
        m_2ndByte = (byte&)fileAsBytes[1];
        m_Handle = (uint64&)fileAsBytes[2];
        m_TypeNameHash = (uint32&)fileAsBytes[10];

        deserStream.currentPtr = (uintptr_t)&fileAsBytes.data()[2 + 8 + 4];
        deserStream.readPastEnd = (uintptr_t)&fileAsBytes.data()[fileAsBytes.size()];
        deserStream.postLoadCallbacks = &deserStream_8;
    }
};
DEFINE_GAME_FUNCTION(JoinManagedObjectAndHandle_mb, 0x142714230, void, __fastcall, (__int64 p_handle, ManagedObject* p_manObj));


class SmthDuringDeserializeManagedObj
{
public:
    static SmthDuringDeserializeManagedObj& GetSingleton() { return *(SmthDuringDeserializeManagedObj*)0x1450A9628; }
};













void DeserializeManagedObject_FullReplacement_originalBehaviorOnly(DeserializationStream* deserStream, ManagedObject** pManObj)
{
	//14270B510


    DEFINE_GAME_FUNCTION(sub_142705D20, 0x142705D20, void, __fastcall, (SmthDuringDeserializeManagedObj* a1, ManagedObject * a2, unsigned __int64 p_handle));
    DEFINE_GAME_FUNCTION(sub_142736FD0, 0x142736FD0, void, __fastcall, (__int64 a1));

    byte byteBeforeHandle;
    uint64 handle;
    uint32 hashInTypeInfosSystem;

    if (deserStream->currentPtr + sizeof(byteBeforeHandle) > deserStream->readPastEnd) DeserializationStream::ReadMore_mb(deserStream);
    deserStream->ReadRawBytes(byteBeforeHandle);
    if (deserStream->currentPtr + sizeof(handle) > deserStream->readPastEnd) DeserializationStream::ReadMore_mb(deserStream);
    deserStream->ReadRawBytes(handle);
    if (deserStream->currentPtr + sizeof(hashInTypeInfosSystem) > deserStream->readPastEnd) DeserializationStream::ReadMore_mb(deserStream);
    deserStream->ReadRawBytes(hashInTypeInfosSystem);

    if (byteBeforeHandle)
    {
        if (*pManObj)
        {
            if (!handle)
                return (*pManObj)->Unk010_Deserialize(deserStream);
            sub_142705D20(&SmthDuringDeserializeManagedObj::GetSingleton(), *pManObj, handle);
        }
        else
        {
            *pManObj = DeserializationStream__CreateObjectForTypehashAndHandle(deserStream, hashInTypeInfosSystem, handle);
        }
        if (handle)
            JoinManagedObjectAndHandle_mb(handle, *pManObj);
    }
    else
    {
        if ( !*pManObj )
            *pManObj = DeserializationStream__CreateObjectForTypehashAndHandle(deserStream, hashInTypeInfosSystem, 0i64);
        if (handle)
        {
            __int64 g_SomeAllocatorDuringParseDatapack = *(__int64*)0x1452585E0;;
            sub_142736FD0(g_SomeAllocatorDuringParseDatapack);
            auto* g_hashmapDuringDeserialize = (ACUHashmap<uint64, ManagedObject*>*)0x14525BAF8;
            g_hashmapDuringDeserialize->Set(handle, *pManObj);
            sub_142736FD0(0i64);
        }
    }
    return (*pManObj)->Unk010_Deserialize(deserStream);
}
#include "SingleObjectOverride.h"
std::optional<ScannedLooseFile> ScanGameObjectLooseFile(const fs::path& targetFilepath)
{
    std::ifstream infile = std::ifstream(targetFilepath, std::ios::binary);
    if (!infile) return {};
    infile.seekg(0, std::ios::end);
    uint32 filesize = (uint32)infile.tellg();
    if (filesize <= 14) return {};
    byte byte0;             infile.seekg(0); infile.read((char*)&byte0, sizeof(byte0));
    byte byte1;             infile.seekg(1); infile.read((char*)&byte1, sizeof(byte1));
    uint64 handle;          infile.seekg(2); infile.read((char*)&handle, sizeof(handle));
    uint32 typeNameHash;    infile.seekg(10); infile.read((char*)&typeNameHash, sizeof(typeNameHash));
    // The game isn't going to check for malformed files,
    // so I'll use some simple criteriums for initial check:
    // I expect the first two bytes to be [0, 1] (is the case
    // for all loose files I checked except Animation) and I also try to look up the TypeInfo
    // by the typename hash (bytes [10:14]).
    if (byte0 != 0) return {};
    if (byte1 != 1) return {};
    TypeInfo* foundTI = ACU::TypeInfos::FindTypeInfoByTypeNameHash(typeNameHash);
    //LOG_DEBUG(SOOLog,
    //    "File: %s, handle: %13llu, typenameHash: %x, typeInfo: %llX, typeName: %s"
    //    , targetFilepath.filename().u8string().c_str()
    //    , handle
    //    , typeNameHash
    //    , foundTI ? *foundTI : 0
    //    , foundTI ? (*foundTI)->typeName : "NO_TYPEINFO"
    //);
    if (!foundTI) return {};
    return ScannedLooseFile{ handle, typeNameHash, *foundTI };
}
fs::path GetSingleObjectOverrideFilepath(uint64 handle);
void DeserializeManagedObject_NormalHandling(DeserializationStream* deserStream, ManagedObject** pManObj
    , byte byteBeforeHandle
    , uint64 handle
    , uint32 hashInTypeInfosSystem)
{
    DEFINE_GAME_FUNCTION(sub_142705D20, 0x142705D20, void, __fastcall, (SmthDuringDeserializeManagedObj * a1, ManagedObject * a2, unsigned __int64 p_handle));
    DEFINE_GAME_FUNCTION(sub_142736FD0, 0x142736FD0, void, __fastcall, (__int64 a1));
    // Weird nested ifs in this section: I'm just reproducing the decompiled code.
    if (byteBeforeHandle)
    {
        if (*pManObj)
        {
            if (!handle)
                return (*pManObj)->Unk010_Deserialize(deserStream);
            sub_142705D20(&SmthDuringDeserializeManagedObj::GetSingleton(), *pManObj, handle);
        }
        else
        {
            *pManObj = DeserializationStream__CreateObjectForTypehashAndHandle(deserStream, hashInTypeInfosSystem, handle);
        }
        if (handle)
            JoinManagedObjectAndHandle_mb(handle, *pManObj);
    }
    else
    {
        if (!*pManObj)
            *pManObj = DeserializationStream__CreateObjectForTypehashAndHandle(deserStream, hashInTypeInfosSystem, 0i64);
        if (handle)
        {
            __int64 g_SomeAllocatorDuringParseDatapack = *(__int64*)0x1452585E0;
            sub_142736FD0(g_SomeAllocatorDuringParseDatapack);
            auto* g_hashmapDuringDeserialize = (ACUHashmap<uint64, ManagedObject*>*)0x14525BAF8;
            g_hashmapDuringDeserialize->Set(handle, *pManObj);
            sub_142736FD0(0i64);
        }
    }
    return (*pManObj)->Unk010_Deserialize(deserStream);
}
// Based on DeserializationStream::From2ndByte() (14270B510)
void DeserializeManagedObject_FullReplacement(DeserializationStream* deserStream, ManagedObject** pManObj)
{
    byte byteBeforeHandle;
    uint64 handle;
    uint32 hashInTypeInfosSystem;

    if (deserStream->currentPtr + sizeof(byteBeforeHandle) > deserStream->readPastEnd) DeserializationStream::ReadMore_mb(deserStream);
    deserStream->ReadRawBytes(byteBeforeHandle);
    if (deserStream->currentPtr + sizeof(handle) > deserStream->readPastEnd) DeserializationStream::ReadMore_mb(deserStream);
    deserStream->ReadRawBytes(handle);
    if (deserStream->currentPtr + sizeof(hashInTypeInfosSystem) > deserStream->readPastEnd) DeserializationStream::ReadMore_mb(deserStream);
    deserStream->ReadRawBytes(hashInTypeInfosSystem);

    fs::path overrideFilepath = GetSingleObjectOverrideFilepath(handle);
    if (overrideFilepath.empty())
    {
        DeserializeManagedObject_NormalHandling(deserStream, pManObj, byteBeforeHandle, handle, hashInTypeInfosSystem);
        return;
    }
    LOG_DEBUG(SOOLog,
        "Overriding single object: handle: %13llu, filepath: %s\n"
        , handle, overrideFilepath.u8string().c_str()
    );

    MockDeserializationStream myDeserStream{ overrideFilepath };

    /*
    I'm going to create an object of the same type, deserialize it from the original data stream,
    then discard it immediately.
    I need to do this in order to make sure the original data stream
    is properly "hollowed out", as if everything is proceeding as it was meant to.
    Then I'll create another object and deserialize it from the overriding loose file,
    Additionally, certain object types do schedule some
    "post deserialization callbacks" and store those callbacks in the "deserialization stream".
    I need to make sure the callbacks from the overriding objects are stored in the original stream
    while the callbacks from original objects are stored (and discarded) in the "fake stream".
    */
    auto DeserializeSeparateNewObjectFromOriginalStreamThenDiscardIt = [&]()
        {
            /*
            At this point there are already ~4 strong references to the shared block
            with the original handle. If I create the discarded object and bind it to the same handle,
            then e.g. the Mesh objects will be fine, but e.g. the TextureMap objects
            seem to be doing some behind-the-scenes caching, bound to _handle_, not to the TextureMap _instance_ somehow
            and only the first deserialization for the same handle will actually determine the texture.
            So I create a fake handle, one that hopefully collides with no other,
            I create a shared block for that handle, and after deserializing I discard the whole thing.
            The largest handle in ACU is:
                115976403725/0x1b00bb7b0d => GIDB_Cell429_LOD0_sid_61765a1f0\GIDB_Cell429_LOD0_sid_61765a1f0.GIDatablock
            So for the discarded handle I use (original_handle + 0x100_0000_0000)
            which hopefully will not produce any collisions.
            UPD: using FindSharedBlock...() with handle==0 seems to allocate a shared block with an arbitrary free handle.
            */
            const uint64 GIVE_ME_A_FREE_HANDLE = 0;
            uint64 handleForTheDiscardedOriginal = GIVE_ME_A_FREE_HANDLE;
            ManagedObject* discardedOriginalObject = DeserializationStream__CreateObjectForTypehashAndHandle(deserStream, hashInTypeInfosSystem, handleForTheDiscardedOriginal);
            auto& discardedShared = (SharedPtrNew<ManagedObject>&)FindSharedBlockByObjectAndIncrementStrongRefcount(*discardedOriginalObject);
            discardedOriginalObject->Unk010_Deserialize(deserStream);
            JoinManagedObjectAndHandle_mb(handleForTheDiscardedOriginal, discardedOriginalObject);
            discardedShared.DecrementStrongRefcount();
        };

    std::swap(myDeserStream.deserStream.postLoadCallbacks, deserStream->postLoadCallbacks);
    DeserializeSeparateNewObjectFromOriginalStreamThenDiscardIt();
    DeserializeManagedObject_NormalHandling(&myDeserStream.deserStream, pManObj, byteBeforeHandle, handle, hashInTypeInfosSystem);
    std::swap(myDeserStream.deserStream.postLoadCallbacks, deserStream->postLoadCallbacks);

}

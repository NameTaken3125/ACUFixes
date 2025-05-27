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

fs::path AbsolutePathInMyDirectory(const fs::path& filenameRel);



DEFINE_GAME_FUNCTION(DeserializationStream__CreateObjectForTypehashAndHandle, 0x1426EB3B0, ManagedObject*, __fastcall, (DeserializationStream* a1, unsigned int p_hashInTypeInfosSystem, unsigned __int64 p_handle));

class CombinedRawBufsReader;
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
class DeserializationStream_PostDeserializationCallbacks
{
public:
    char pad_0000[128]; //0x0000
    std::array<DeserializationStream_ArrPostloadCallbacks, 16> callbackStages_mb; //0x0080
    char pad_0180[8]; //0x0180
}; //Size: 0x0188
class DeserializationStream
{
public:
    char pad_0000[8]; //0x0000
    DeserializationStream_PostDeserializationCallbacks* postLoadCallbacks; //0x0008
    uint64 p10; //0x0010
    char pad_0018[8]; //0x0018
    void* readStart_from2ndByte; //0x0020
    uintptr_t currentPtr; //0x0028
    uintptr_t readPastEnd; //0x0030
    class DeserializationStream_38* p_38; //0x0038
    char pad_0040[24]; //0x0040
    uint32 dword_58; //0x0058
    char pad_005C[4]; //0x005C
    CombinedRawBufsReader* combinedRawBufsReader; //0x0060
    class UsedDuringLoadDataPackGetLinkedObjects* datapackParser; //0x0068
    char pad_0070[8]; //0x0070
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
    DeserializationStream_PostDeserializationCallbacks deserStream_8;
    std::vector<byte> fileAsBytes;

    fs::path m_Filepath;
    byte m_1stByte; // == 0
    byte m_2ndByte; // == 1
    uint64 m_Handle;
    uint32 m_TypeNameHash;

    MockDeserializationStream(const fs::path& filepath)
        : m_Filepath(filepath)
        , deserStream{ 0 }
        , deserStream_8{ 0 }
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
std::unordered_map<uint64, fs::path> g_SingleObjectOverride_FilepathRelByHandle;
std::optional<uint64> FindHandleFromLooseFile(const fs::path& targetFilepath)
{
    std::ifstream infile = std::ifstream(targetFilepath, std::ios::binary);
    if (!infile) return {};
    infile.seekg(0, std::ios::end);
    uint32 filesize = (uint32)infile.tellg();
    if (filesize < 10) return {};
    infile.seekg(2);
    uint64 handle{ 0 };
    infile.read((char*)&handle, sizeof(handle));
    return handle;
}
fs::path GetPathInAssetModsFolder(const fs::path& pathRel)
{
    return AbsolutePathInMyDirectory(AbsolutePathInMyDirectory("plugins\\AssetOverrides") / pathRel);
}
std::vector<fs::path> SingleObjectOverride_InitList_GatherLooseFilePaths()
{
    std::vector<fs::path> folders = {
        "_WPN_Excalibur\\LooseFiles",
        "_WPN_Gungnir\\LooseFiles",
        "_WPN_SwordOfAltair\\LooseFiles",
        "_WPN_Mjolnir\\LooseFiles",
        "_WPN_BasimSword\\LooseFiles",
        "_WPN_Joyeuse\\LooseFiles",
        "_WPN_ConnorsTomahawk\\LooseFiles",

        "_AtomGraph\\LooseFiles",
    };
    std::vector<fs::path> looseFiles;
    looseFiles.reserve(64);
    for (auto& dirRel : folders)
    {
        fs::path absoluteDir = AbsolutePathInMyDirectory(AbsolutePathInMyDirectory("plugins\\AssetOverrides") / dirRel);
        try {
            for (const auto& entry : fs::directory_iterator(absoluteDir)) {
                if (entry.is_regular_file()) {
                    looseFiles.push_back(entry.path());
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            LOG_DEBUG(
                SOOLog
                , "Filesystem error: %s\n"
                , e.what()
            );
        }
    }
    return looseFiles;
}
void SingleObjectOverride_InitList()
{
    std::vector<fs::path> looseFiles = SingleObjectOverride_InitList_GatherLooseFilePaths();
    for (auto& filepath : looseFiles)
    {
        fs::path absolutePath = AbsolutePathInMyDirectory("plugins\\AssetOverrides") / filepath;
        std::optional<uint64> retrievedHandle = FindHandleFromLooseFile(absolutePath);
        if (!retrievedHandle) continue;
        g_SingleObjectOverride_FilepathRelByHandle[*retrievedHandle] = absolutePath;
    }
}
static fs::path GetOverrideFilepath(uint64 handle)
{
    if (!handle) return {};
    auto foundIt = g_SingleObjectOverride_FilepathRelByHandle.find(handle);
    if (foundIt == g_SingleObjectOverride_FilepathRelByHandle.end()) return {};
    return foundIt->second;
}
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

    fs::path overrideFilepath = GetOverrideFilepath(handle);
    if (overrideFilepath.empty())
    {
        DeserializeManagedObject_NormalHandling(deserStream, pManObj, byteBeforeHandle, handle, hashInTypeInfosSystem);
        return;
    }


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

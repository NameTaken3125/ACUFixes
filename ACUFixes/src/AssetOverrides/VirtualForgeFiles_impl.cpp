#include "pch.h"

#include "VirtualForgeFiles_impl.h"


DEFINE_GAME_FUNCTION(Allocate_mb, 0x1426F3760, void*, __fastcall, (__int64 p_numBytes, uint32 p_alignment_mb, void* currentHeap_mb));
DEFINE_GAME_FUNCTION(Allocate2, 0x1426D9A10, void*, __fastcall, (__int64 p_numBytes, unsigned int a2, void* resultOf0x1426F3760));
DEFINE_GAME_FUNCTION(BigArray__usedInEquipmentUnlocker, 0x142726010, void*, __fastcall, (void* bigArray, void* p_newBuffer, int p_prevSize, int a4, int a5, char a6, char a7, char a8, unsigned __int8 a9));

DEFINE_GAME_FUNCTION(stringHashCRC32_Cstring, 0x142759CF0, uint32, __fastcall, (char*));


namespace ACU::Memory
{
    template<typename T>
    T* ACUAllocateDefaultConstruct()
    {
        void* placementAddr = ACUAllocateBytes(sizeof(T), alignof(T));
        return new(placementAddr)T;
    }
}
template<typename T>
void BigArrayReserve(BigArray<T>& arr, uint32 newCapacity)
{
    // Example implementation of `BigArray<ForgeFile_DatapackPrefetchInfo>::Reserve()` at 142732BB0.
    uint32 prevCap = arr.GetCapacity();
    if (prevCap >= newCapacity) { return; }
    uint32 newBytesize = sizeof(T) * newCapacity;
    void* v6 = Allocate_mb(newBytesize, 0xFFFFFFFF, *(void**)0x1452585D0);
    void* newBuffer = Allocate2(newBytesize, 1, v6);
    if (newBuffer)
    {
        uint32 prevSize = arr.size;
        if (prevSize)
        {
            for (uint32 i = 0; i < prevSize; i++)
            {
                T& oldElem = arr[i];
                void* newElemPlacement = &((T*)newBuffer)[i];
                new (newElemPlacement) T(std::move(oldElem));
            }
        }
        BigArray__usedInEquipmentUnlocker(
            (void*)&arr, newBuffer, prevSize, newCapacity
            , 16 // ????
            , 1, 1, 1, 1);
    }
}
template<typename T>
T* BigArrayAppend(BigArray<T>& arr)
{
    BigArrayReserve(arr, arr.size + 1);
    T* newElemPlacement = &arr[arr.size++];
    return (new (newElemPlacement) T);
}


ForgeFile_38* AllocateVirtualForgeContents(ForgeFile& newForge)
{
    ForgeFile_38* newDatapacksDesc = ACU::Memory::ACUAllocateDefaultConstruct<ForgeFile_38>();
    newDatapacksDesc->p30 = &newForge.stru_240;
    newDatapacksDesc->qword_38 = newForge.header.qword_10_eq0x41A;
    return newDatapacksDesc;
}
void SetForgefilePathStrings(ForgeFile& newForge, const fs::path& targetFilepath)
{
    fs::path abspath = targetFilepath;
    std::string s = abspath.filename().string();
    strncpy_s(newForge.filename, 255, s.c_str(), 254);
    newForge.filename[255] = 0;

    newForge.dirpathIfNotRootFolder_mb = ACU::Memory::ACUAllocateString(abspath.parent_path().string() + "\\");
}
DEFINE_GAME_FUNCTION(sub_14276BC70, 0x14276BC70, __int64, __fastcall, (ForgeFile_240* a1, __int64 p_offsetInForgeFile, unsigned int a3));
DEFINE_GAME_FUNCTION(ForgeFile__RecalculateNameHash_mb, 0x142721B50, __int64, __fastcall, (ForgeFile* a1));
DEFINE_GAME_FUNCTION(sub_1427327B0, 0x1427327B0, int, __fastcall, (ForgeFile* a1));
void WhenOpenedVirtualForge_Initialize(ForgeFile& newForge, char a3_from14272EE10)
{
    // Implementation taken from 0x14272EF14.

    sub_14276BC70(&newForge.stru_240, 0, 2);

    // Instead of reading the header (`.forge` files start with "scimitar" string and then some stuff),
    // I fill out the header manually.
    {
        newForge.header.metadataPackHandle = 0x10;
        newForge.header.dword_C_eq0x1b = 0x1B;
        newForge.header.qword_10_eq0x41A = 0x41A;
        strncpy_s(newForge.header.str_scimitar, "scimitar", 9);
        newForge.header.byte_20 = 3; // if <= 2 then `.PrefetchInfo` file is required, it kinda seems.
    }

    {
        ForgeFile__RecalculateNameHash_mb(&newForge);
    }

    newForge.someNull_2F8 = 0;

    if (a3_from14272EE10)
    {
        sub_1427327B0(&newForge);
    }
}
DEFINE_GAME_FUNCTION(ForgeFile_240__OpenFile_mb, 0x142764C90, char, __fastcall, (ForgeFile_240* a1, const char* p_filepath, unsigned int p_1whenOpenForge));


ForgeFileEntry* MakeNewForgeFileEntry_impl(uint64 targetHandle, const fs::path& absoluteFilepath, LoadPriority priority)
{
    const uint32 datapackSize = (uint32)fs::file_size(absoluteFilepath);
    auto* fm = ForgeManager::GetSingleton();
    if (!fm) { return {}; }
    ForgeFile* newForge = ACU::Memory::ACUAllocateDefaultConstruct<ForgeFile>();
    newForge->someLock++;
    SetForgefilePathStrings(*newForge, absoluteFilepath);
    ForgeFile_DatapackPrefetchInfo* singleDatapackPrefetchInfo = BigArrayAppend(newForge->datapacksPrefetchInfoAscendingHandles);
    singleDatapackPrefetchInfo->firstHandle = targetHandle;
    singleDatapackPrefetchInfo->offsetInPrefetchData = 0;
    singleDatapackPrefetchInfo->prefetchDataSize = 4;
    singleDatapackPrefetchInfo->isCompressed_mb = false;
    newForge->prefetchData = ACU::Memory::ACUAllocateBytes(4, 0x10);
    std::memset(newForge->prefetchData, 0, 4);

    ForgeFile_38* newDatapacksDesc = AllocateVirtualForgeContents(*newForge);
    newForge->p38 = newDatapacksDesc;

    DatapackDescriptor* singleDatapackDesc = BigArrayAppend(newDatapacksDesc->arrDatapackDescs);
    singleDatapackDesc->handleFirstInDatapack = targetHandle;
    singleDatapackDesc->offsetInForgeFile = 0;
    singleDatapackDesc->packedSize = datapackSize;
    newDatapacksDesc->hashmap_datapackIdxByFirstHandle.Set(singleDatapackDesc->handleFirstInDatapack, 0);

    ForgeFileEntry* newForgeEntry = ACU::Memory::ACUAllocateDefaultConstruct<ForgeFileEntry>();
    newForgeEntry->forgeIdx_mb = ++fm->nextForgeIdx;
    newForgeEntry->forgeContentsDescriptor = newForge;

    const bool isSuccessfullyOpened = ForgeFile_240__OpenFile_mb(&newForge->stru_240, absoluteFilepath.string().c_str(), 1);
    if (isSuccessfullyOpened)
    {
        WhenOpenedVirtualForge_Initialize(*newForge, 1);
    }

    switch (priority)
    {
    case LoadPriority::Lowest:
        ACU::Memory::SmallArrayAppend(fm->forges, newForgeEntry);
        break;
    case LoadPriority::Highest:
    default:
        ACU::Memory::SmallArrayInsert(fm->forges, newForgeEntry, 0);
        break;
    }

    newForgeEntry->refcount_mb = 1;
    if (!isSuccessfullyOpened)
    {
        ForgeManager__DecrementForgeEntryRefcount_mb(fm, newForgeEntry->forgeIdx_mb);
        return {};
    }

    return newForgeEntry;
}
ForgeFileEntry* MakeNewForgeFileEntry(uint64 targetHandle, const fs::path& absoluteFilepath, LoadPriority priority)
{
    try
    {
        return MakeNewForgeFileEntry_impl(targetHandle, absoluteFilepath, priority);
    }
    catch (const fs::filesystem_error&)
    {
        LOG_DEBUG(VirtualForgesLog
            , "[error] Failed to create virtual forge: \"%s\""
            , absoluteFilepath.u8string().c_str()
        );
    }
    return nullptr;
}
bool IsForgeAlive(ForgeManager& fm, ForgeIndex_t forgeIdx)
{
    auto& forges = fm.forges;
    return std::find_if(forges.begin(), forges.end(), [&](ForgeFileEntry* forgeEntry)
        {
            return forgeEntry->forgeIdx_mb == forgeIdx;
        }) != forges.end();
}


#include "pch.h"

#include "ACU/ForgeManager.h"
#include "ACU/ManagedPtrs/ManagedPtrs.h"
#include "ACU/Memory/ACUAllocs.h"
#include "AssetOverrides.h"
#include "Common_Plugins/Common_PluginSide.h"
#include "Serialization/Serialization.h"
#include "Serialization/NumericAdapters.h"
#include "Serialization/BooleanAdapter.h"
#include "Serialization/FilepathAdapter.h"
#include "Serialization/ToFromFile/ToFromFile.h"

#include "Handles.h"

#include "ImGuiCTX.h"

#include "MyLog.h"
static DEFINE_LOGGER_CONSOLE_AND_FILE(VirtualForgesLog, "[AssetOverrides]");



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

DEFINE_GAME_FUNCTION(ForgeManager__DecrementForgeEntryRefcount_mb, 0x142721FE0, void, __fastcall, (ForgeManager* a1, int p_forgeIdx));

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

void WhenNewForgeEntryWasJustAdded_ApplyCustomSorting(AllRegisters* params);
void WhenGatheringPrefetchInfoForDatapack_FindOriginalPrefetchInfo(AllRegisters* params);

// Pay no attention to this for now.
enum class LoadPriority
{
    Lowest = 0,
    Highest = 1,
};
using ForgeIndex_t = uint32;
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

std::optional<uint64> FindFirstHandleInDatapack(const fs::path& targetFilepath);

struct ResultForDetectedDatapack_LooksOk
{
    uint64 m_Handle;
};
struct ResultForDetectedDatapack_LooksInvalid
{
    std::u8string m_ErrorMsg;
};
struct ResultForDetectedDatapack_LookedOkButFailedToCreateVirtualForge
{
    std::u8string m_ErrorMsg;
};
using ResultForDetectedDatapack = std::variant<
    ResultForDetectedDatapack_LooksOk
    , ResultForDetectedDatapack_LooksInvalid
    , ResultForDetectedDatapack_LookedOkButFailedToCreateVirtualForge
>;
class DetectedDatapack
{
public:
    fs::path m_AbsolutePath;
    ResultForDetectedDatapack m_Result;
    std::optional<ForgeIndex_t> m_ForgeIdx;

    ResultForDetectedDatapack_LooksOk* GetContentsIfDetected()
    {
        if (auto* datapackOk = std::get_if<ResultForDetectedDatapack_LooksOk>(&m_Result)) return datapackOk;
        return nullptr;
    }
    std::u8string* GetError()
    {
        if (auto* datapackInvalid = std::get_if<ResultForDetectedDatapack_LooksInvalid>(&m_Result)) return &datapackInvalid->m_ErrorMsg;
        if (auto* datapackFailedToLoad = std::get_if<ResultForDetectedDatapack_LookedOkButFailedToCreateVirtualForge>(&m_Result)) return &datapackFailedToLoad->m_ErrorMsg;
        return nullptr;
    }
    ~DetectedDatapack()
    {
        auto* fm = ForgeManager::GetSingleton();
        if (!fm) return;
        if (m_ForgeIdx && IsForgeAlive(*fm, *m_ForgeIdx))
        {
            ForgeManager__DecrementForgeEntryRefcount_mb(fm, *m_ForgeIdx);
        }
    }
};
struct ResultForDetectedLooseFile_LooksOk
{
    uint64 m_Handle;
    std::reference_wrapper<TypeInfo> m_TypeInfo;
};
struct ResultForDetectedLooseFile_LooksInvalid
{
    std::u8string m_ErrorMsg;
};
struct ResultForDetectedLooseFile_LookedOkButFailedToLoad
{
    std::u8string m_ErrorMsg;
};
using ResultForDetectedLooseFile = std::variant<
    ResultForDetectedLooseFile_LooksOk
    , ResultForDetectedLooseFile_LooksInvalid
    , ResultForDetectedLooseFile_LookedOkButFailedToLoad
>;
class DetectedLooseFile
{
public:
    fs::path m_AbsolutePath;
    ResultForDetectedLooseFile m_Result;
    std::u8string* GetError()
    {
        if (auto* datapackInvalid = std::get_if<ResultForDetectedLooseFile_LooksInvalid>(&m_Result)) return &datapackInvalid->m_ErrorMsg;
        if (auto* datapackFailedToLoad = std::get_if<ResultForDetectedLooseFile_LookedOkButFailedToLoad>(&m_Result)) return &datapackFailedToLoad->m_ErrorMsg;
        return nullptr;
    }
};
std::vector<std::unique_ptr<DetectedDatapack>> GetDatapacksFromFolder(const fs::path& modFolder)
{
    ImGuiTextBuffer buf;
    std::string extension = ".data";
    std::vector<std::unique_ptr<DetectedDatapack>> result;
    try {
        for (const auto& entry : fs::directory_iterator(modFolder)) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
                LOG_DEBUG(
                    VirtualForgesLog
                    , "  Datapack: %s\n"
                    , entry.path().filename().u8string().c_str()
                );
                std::optional<uint64> correspondingHandle = FindFirstHandleInDatapack(entry);
                if (correspondingHandle)
                {
                    LOG_DEBUG(
                        VirtualForgesLog
                        , "    Handle: %llu\n"
                        , *correspondingHandle
                    );
                    result.emplace_back(std::make_unique<DetectedDatapack>(
                        entry,
                        ResultForDetectedDatapack_LooksOk{ *correspondingHandle }
                    ));
                }
                else
                {
                    buf.clear();
                    buf.appendf("Couldn't retrieve handle from \"%s\"", entry.path().filename().u8string().c_str());
                    LOG_DEBUG(VirtualForgesLog,
                        "[error]%s\n"
                        , buf.c_str()
                    );
                    result.emplace_back(std::make_unique<DetectedDatapack>(
                        entry,
                        ResultForDetectedDatapack_LooksInvalid{ (const char8_t*)buf.c_str() }
                    ));
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        LOG_DEBUG(
            VirtualForgesLog
            , "Filesystem error: %s\n"
            , e.what()
        );
        result.clear();
    }
    return result;
}
#include "SingleObjectOverride.h"
std::vector<std::unique_ptr<DetectedLooseFile>> GetLooseFilesFromFolder(const fs::path& modFolder)
{
    ImGuiTextBuffer buf;
    fs::path looseFilesFolder = modFolder / "LooseFiles";
    std::vector<std::unique_ptr<DetectedLooseFile>> result;
    if (!fs::is_directory(looseFilesFolder)) return result;
    try {
        for (const auto& entry : fs::directory_iterator(looseFilesFolder)) {
            if (entry.is_regular_file()) {
                LOG_DEBUG(
                    VirtualForgesLog
                    , "  LooseFile: %s\n"
                    , entry.path().filename().u8string().c_str()
                );
                std::optional<ScannedLooseFile> scannedLooseFile = ScanGameObjectLooseFile(entry);
                if (scannedLooseFile)
                {
                    LOG_DEBUG(VirtualForgesLog,
                        "    Handle : %12llu, Typename: %s\n"
                        , scannedLooseFile->m_Handle
                        , scannedLooseFile->m_CorrespondingTypeInfo.get().typeName
                    );
                    result.emplace_back(std::make_unique<DetectedLooseFile>(
                        entry,
                        ResultForDetectedLooseFile_LooksOk{ scannedLooseFile->m_Handle, scannedLooseFile->m_CorrespondingTypeInfo }
                    ));
                }
                else
                {
                    buf.clear();
                    buf.appendf("\"%s\" does not look like a valid game object file or is unsupported.", entry.path().filename().u8string().c_str());
                    LOG_DEBUG(VirtualForgesLog,
                        "[error]%s\n"
                        , buf.c_str()
                    );
                    result.emplace_back(std::make_unique<DetectedLooseFile>(
                        entry,
                        ResultForDetectedLooseFile_LooksInvalid{ (const char8_t*)buf.c_str() }
                    ));
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        LOG_DEBUG(
            VirtualForgesLog
            , "Filesystem error: %s\n"
            , e.what()
        );
        result.clear();
    }
    return result;
}
class AssetMod
{
public:
    fs::path m_ModFolder;
    std::u8string m_ModName;
    std::vector<std::unique_ptr<DetectedDatapack>> m_Datapacks;
    std::vector<std::unique_ptr<DetectedLooseFile>> m_LooseFiles;
    bool m_IsActiveInLoadOrder = false;
    bool m_WasLoadedFromLoadOrderButNotFoundAnymore = false;

    bool m_IsErrorInDatapacks = false;
    bool m_IsErrorInLooseFiles = false;
    bool m_IsError = false;

    AssetMod(const fs::path& modFolder_)
        : m_ModFolder(modFolder_)
        , m_ModName(modFolder_.filename().u8string())
    {}
    void GatherDatapacksAndLooseFiles()
    {
        m_Datapacks = GetDatapacksFromFolder(m_ModFolder);
        m_LooseFiles = GetLooseFilesFromFolder(m_ModFolder);
        m_IsErrorInDatapacks = std::find_if(m_Datapacks.begin(), m_Datapacks.end(), [&](std::unique_ptr<DetectedDatapack>& datapack)
            {
                auto* error = datapack->GetError();
                if (!error) return false;
                LOG_DEBUG(VirtualForgesLog,
                    "[error]Not loading the folder \"%s\" because: %s\n"
                    , m_ModFolder.filename().u8string().c_str()
                    , error->c_str()
                );
                return true;
            }) != m_Datapacks.end();
        m_IsErrorInLooseFiles = std::find_if(m_LooseFiles.begin(), m_LooseFiles.end(), [&](std::unique_ptr<DetectedLooseFile>& looseFile)
            {
                auto* error = looseFile->GetError();
                if (!error) return false;
                LOG_DEBUG(VirtualForgesLog,
                    "[error]Not loading the folder \"%s\" because: %s\n"
                    , m_ModFolder.filename().u8string().c_str()
                    , error->c_str()
                );
                return true;
            }) != m_LooseFiles.end();
        m_IsError = m_IsErrorInDatapacks || m_IsErrorInLooseFiles;
    }
};

std::optional<uint16> GetPrefetchInfoIndex(ForgeFileEntry& forge, uint64 handle)
{
    auto& prefetchInfos = forge.forgeContentsDescriptor->datapacksPrefetchInfoAscendingHandles;
    int left = 0;
    int right = prefetchInfos.size;
    while (left <= right)
    {
        uint32 mid = (left + right) / 2;
        const ForgeFile_DatapackPrefetchInfo& curInfo = prefetchInfos[mid];
        if (curInfo.firstHandle == handle)
        {
            return mid;
        }
        if (curInfo.firstHandle < handle)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }
    return {};
};
void ResetPrefetchingInfoForVirtualForge(ForgeFile& moddedForge, uint64 targetHandle)
{
    if (moddedForge.prefetchData)
    {
        ACU::Memory::ACUDeallocateBytes((byte*)moddedForge.prefetchData);
    }
    ForgeFile_DatapackPrefetchInfo* singleDatapackPrefetchInfo = &moddedForge.datapacksPrefetchInfoAscendingHandles[0];
    singleDatapackPrefetchInfo->firstHandle = targetHandle;
    singleDatapackPrefetchInfo->offsetInPrefetchData = 0;
    singleDatapackPrefetchInfo->prefetchDataSize = 4;
    singleDatapackPrefetchInfo->isCompressed_mb = false;
    moddedForge.prefetchData = ACU::Memory::ACUAllocateBytes(4, 0x10);
    std::memset(moddedForge.prefetchData, 0, 4);
}
void StealPrefetchingInfoIntoVirtualForge(ForgeFile& cloneTo, ForgeFile& cloneFrom, uint16 prefetchInfoIdx)
{
    ForgeFile_DatapackPrefetchInfo& originalPrefetchInfo = cloneFrom.datapacksPrefetchInfoAscendingHandles[prefetchInfoIdx];
    if (cloneTo.prefetchData)
    {
        ACU::Memory::ACUDeallocateBytes((byte*)cloneTo.prefetchData);
    }
    byte* duplicatedOriginalPrefetchData = ACU::Memory::ACUAllocateBytes(originalPrefetchInfo.prefetchDataSize, 16);
    std::memcpy(
        duplicatedOriginalPrefetchData,
        &cloneFrom.prefetchData[originalPrefetchInfo.offsetInPrefetchData],
        originalPrefetchInfo.prefetchDataSize
    );
    cloneTo.prefetchData = duplicatedOriginalPrefetchData;
    // I'm assuming that in my "virtual forges" there is only a single datapack.
    ForgeFile_DatapackPrefetchInfo& singularDatapackPrefetchInfo = cloneTo.datapacksPrefetchInfoAscendingHandles[0];
    singularDatapackPrefetchInfo.isCompressed_mb = originalPrefetchInfo.isCompressed_mb;
    singularDatapackPrefetchInfo.prefetchDataSize = originalPrefetchInfo.prefetchDataSize;
    singularDatapackPrefetchInfo.offsetInPrefetchData = 0;
}
namespace ImGui
{
void CopyToClipboardOnClick(const char* s)
{
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to copy to clipboard");
    if (ImGui::IsItemClicked()) ImGui::SetClipboardText(s);
}
}
class AssetModlist
{
public:
    std::vector<std::unique_ptr<AssetMod>> m_Mods;

    std::pair<ForgeFileEntry*, uint16> FindOriginalPrefetchingInfoForHandle(uint64 handle)
    {
        auto* fm = ForgeManager::GetSingleton();
        std::vector<DetectedDatapack*> gatheredLoadOrder = GatherDatapackOrder();
        std::vector<ForgeFileEntry*> orderedNonmoddedForges;
        orderedNonmoddedForges.reserve(gatheredLoadOrder.size());
        for (ForgeFileEntry* forge : fm->forges)
        {
            uint32 forgeIdx = forge->forgeIdx_mb;
            const bool isOneOfMyVirtualForges =
                std::find_if(gatheredLoadOrder.begin(), gatheredLoadOrder.end(), [forgeIdx](DetectedDatapack* myDatapack)
                    { return myDatapack->m_ForgeIdx && myDatapack->m_ForgeIdx == forgeIdx; }) != gatheredLoadOrder.end();
            if (isOneOfMyVirtualForges) continue;
            orderedNonmoddedForges.push_back(forge);
        }
        for (ForgeFileEntry* nonmoddedForge : orderedNonmoddedForges)
        {
            if (!nonmoddedForge->forgeContentsDescriptor->p38) continue;
            if (int* foundDatapackIdx = nonmoddedForge->forgeContentsDescriptor->p38->hashmap_datapackIdxByFirstHandle.Get(handle))
            {
                std::optional<uint16> prefetchInfoIdx = GetPrefetchInfoIndex(*nonmoddedForge, handle);
                if (prefetchInfoIdx)
                {
                    return { nonmoddedForge, *prefetchInfoIdx };
                }
            }
        }
        return { nullptr, -1 };
    }
    void FindAndFormatOriginalPrefetchingInfoForHandle(ImGuiTextBuffer& buf, uint64 handle)
    {
        auto [nonmoddedForge, prefetchInfoIdx] = FindOriginalPrefetchingInfoForHandle(handle);
        if (!nonmoddedForge)
        {
            buf.append("No prefetching info found in game's native forges.");
            return;
        }
        ForgeFile_DatapackPrefetchInfo& prefetchInfo = nonmoddedForge->forgeContentsDescriptor->datapacksPrefetchInfoAscendingHandles[prefetchInfoIdx];
        uintptr_t prefetchInfoAddr = (uintptr_t)nonmoddedForge->forgeContentsDescriptor->prefetchData + prefetchInfo.offsetInPrefetchData;
        buf.appendf(
            "Found prefetching info in forge #%d (%s) at idx %hd (size=%u, isCompressed=%s, addr=%llX)"
            , nonmoddedForge->forgeIdx_mb
            , nonmoddedForge->forgeName
            , prefetchInfoIdx
            , prefetchInfo.prefetchDataSize
            , prefetchInfo.isCompressed_mb ? "true" : "false"
            , prefetchInfoAddr
        );
        return;
    }

    bool IsModHasConflicts(AssetMod& lhs, AssetMod& rhs)
    {
        for (auto& Ldatapack : lhs.m_Datapacks)
        {
            auto* LdatapackOk = std::get_if<ResultForDetectedDatapack_LooksOk>(&Ldatapack->m_Result);
            if (!LdatapackOk) continue;
            uint64 Lhandle = LdatapackOk->m_Handle;
            for (auto& Rdatapack : rhs.m_Datapacks)
            {
                auto* RdatapackOk = std::get_if<ResultForDetectedDatapack_LooksOk>(&Rdatapack->m_Result);
                if (!RdatapackOk) continue;
                uint64 Rhandle = RdatapackOk->m_Handle;
                if (Rhandle == Lhandle) return true;
            }
        }
        return false;
    }
    struct Popup_DatapackDetails
    {
        bool isOpen = true;
        AssetMod* selectedMod = nullptr;
        DetectedDatapack* selectedDatapack = nullptr;
        ImGuiID ID{};
    } m_PDD;
    void OpenPopup_DatapackDetails(AssetMod& selectedMod, DetectedDatapack& datapack)
    {
        m_PDD.isOpen = true;
        m_PDD.selectedMod = &selectedMod;
        m_PDD.selectedDatapack = &datapack;
        ImGui::OpenPopup(m_PDD.ID);
    }
    void DrawPopup_DatapackDetails()
    {
        static ImGuiTextBuffer buf;
        // PDD == Popup Datapack Details
        const char* pdd_IDstr = "Datapack details";
        m_PDD.ID = ImGui::GetID(pdd_IDstr);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
        if (ImGuiCTX::PopupModal _pdd{ pdd_IDstr, &m_PDD.isOpen })
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
                ImGui::CloseCurrentPopup();
            bool isSelectedModStillAlive = false;
            for (size_t i = 0; i < m_Mods.size(); i++)
            {
                auto& mod = m_Mods[i];
                if (mod.get() == m_PDD.selectedMod)
                {
                    for (size_t j = 0; j < mod->m_Datapacks.size(); j++)
                    {
                        auto& dp = mod->m_Datapacks[j];
                        if (dp.get() == m_PDD.selectedDatapack)
                        {
                            isSelectedModStillAlive = true;
                            break;
                        }
                    }
                }
            }
            if (!isSelectedModStillAlive)
            {
                m_PDD.selectedMod = nullptr;
                m_PDD.selectedDatapack = nullptr;
            }
            if (!m_PDD.selectedMod || !m_PDD.selectedDatapack)
            {
                ImGui::Text("No datapack selected.");
                return;
            }
            uint64 selectedHandle{ 0 };
            if (auto* selectedDatapackOk = std::get_if<ResultForDetectedDatapack_LooksOk>(&m_PDD.selectedDatapack->m_Result))
            {
                selectedHandle = selectedDatapackOk->m_Handle;
            }

            ImGui::Text("Selected datapack: [%s] %s"
                , m_PDD.selectedMod->m_ModName.c_str(), m_PDD.selectedDatapack->m_AbsolutePath.filename().u8string().c_str()
            );
            buf.clear();
            buf.appendf(
                "Handle: %llu/%llX => %s"
                , selectedHandle, selectedHandle
                , ACU::Handles::HandleToText(selectedHandle).c_str()
            );
            ImGui::Text(buf.c_str());
            ImGui::CopyToClipboardOnClick(buf.c_str());
            buf.clear();
            FindAndFormatOriginalPrefetchingInfoForHandle(buf, selectedHandle);
            ImGui::Text(buf.c_str());
            ImGui::CopyToClipboardOnClick(buf.c_str());
            auto DrawConflicts = [&](const char* description, bool isLeft, size_t startFromIdx) -> size_t
                {
                    ImGuiChildFlags conflictPaneFlags =
                        ImGuiChildFlags_Borders
                        ;
                    float conflictPaneWidth = 0;
                    ImVec4 color_paneHeader;
                    if (isLeft)
                    {
                        conflictPaneFlags |= ImGuiChildFlags_ResizeX;
                        conflictPaneWidth = -ImGui::GetWindowWidth() * 0.5f;
                        color_paneHeader = ImVec4(0.425f, 0.780f, 0.392f, 1.000f);
                    }
                    else
                    {
                        color_paneHeader = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                    }
                    if (ImGuiCTX::WindowChild _overridesOthers{ description, ImVec2(conflictPaneWidth, 0), conflictPaneFlags })
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, color_paneHeader);
                        ImGui::Text(description);
                        ImGui::PopStyleColor();
                        for (size_t modIdx = startFromIdx; modIdx < m_Mods.size(); modIdx++)
                        {
                            auto& mod = m_Mods[modIdx];
                            if (mod.get() == m_PDD.selectedMod) return modIdx;
                            if (!mod->m_IsActiveInLoadOrder) continue;
                            //if (mod->m_IsError) continue;
                            for (auto& dp : mod->m_Datapacks)
                            {
                                if (auto* datapackOk = std::get_if<ResultForDetectedDatapack_LooksOk>(&dp->m_Result))
                                {
                                    uint64 handle = datapackOk->m_Handle;
                                    if (datapackOk->m_Handle == selectedHandle)
                                    {
                                        buf.clear();
                                        buf.appendf("[%s] %s", mod->m_ModName.c_str(), dp->m_AbsolutePath.filename().u8string().c_str());
                                        ImGui::Text(buf.c_str());
                                    }
                                }
                            }
                        }
                    }
                    return m_Mods.size();
                };
            size_t lastCheckedModIdx;
            lastCheckedModIdx = DrawConflicts("Overrides other datapacks (active mods only):", true, 0);
            ImGui::SameLine();
            lastCheckedModIdx = DrawConflicts("Overridden by others (active mods only):", false, lastCheckedModIdx + 1);
        }
    }
    bool IsDatapackOverridden_(DetectedDatapack& datapack)
    {
        auto* datapackOk = std::get_if<ResultForDetectedDatapack_LooksOk>(&datapack.m_Result);
        if (!datapackOk) return false;
        uint64 handle = datapackOk->m_Handle;
        for (size_t i = m_Mods.size() - 1; i != static_cast<size_t>(-1); --i)
        {
            auto& mod = m_Mods[i];
            if (!mod->m_IsActiveInLoadOrder) continue;
            if (mod->m_IsError) continue;
            for (auto& dp : mod->m_Datapacks)
            {
                if (auto* datapackOk = std::get_if<ResultForDetectedDatapack_LooksOk>(&dp->m_Result))
                {
                    if (datapackOk->m_Handle == handle)
                    {
                        if (dp.get() != &datapack) return true;
                    }
                }
            }
        }
        return false;
    }
    AssetMod* FindHigherPriorityOverride(DetectedDatapack& datapack)
    {
        auto* datapackOk = std::get_if<ResultForDetectedDatapack_LooksOk>(&datapack.m_Result);
        if (!datapackOk) return nullptr;
        uint64 handle = datapackOk->m_Handle;
        for (size_t i = m_Mods.size() - 1; i != static_cast<size_t>(-1); --i)
        {
            auto& mod = m_Mods[i];
            if (!mod->m_IsActiveInLoadOrder) continue;
            if (mod->m_IsError) continue;
            for (auto& dp : mod->m_Datapacks)
            {
                if (auto* datapackOk = std::get_if<ResultForDetectedDatapack_LooksOk>(&dp->m_Result))
                {
                    if (datapackOk->m_Handle == handle)
                    {
                        if (dp.get() == &datapack) return nullptr;
                        else return mod.get();
                    }
                }
            }
        }
        return nullptr;
    }
    std::optional<size_t> FindModIdxWithTopPriorityDatapack(uint64 handle)
    {
        for (size_t i = m_Mods.size(); i != static_cast<size_t>(-1); --i)
        {
            auto& mod = m_Mods[i];
            for (auto& dp : mod->m_Datapacks)
            {
                if (auto* datapackOk = std::get_if<ResultForDetectedDatapack_LooksOk>(&dp->m_Result))
                {
                    if (datapackOk->m_Handle == handle)
                    {

                    }
                }
            }
        }
        return {};
    }
    void DrawMenu()
    {
        static bool doShowConflicts = false;
        if (ImGui::Button("Save load order"))
        {
            LoadOrder_Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh load order"))
        {
            InitFromLoadOrder();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Show conflicts", &doShowConflicts);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(
                "Highlight the mods that have potential conflicts with the selected mod.\n"
                "Green means lower priority, red means higher priority.\n"
                "This is not necessarily a problem: if e.g. two different mods both replace\n"
                "the Ezio outfit, then they are of course \"in conflict\", but only one\n"
                "will be used at a time, so you might not face any issues."
            );
        ImGui::Separator();
        ImGui::Text("Load Order (drag to reorder, right click on a mod to toggle)");
        ImGui::Separator();
        static ImGuiTextBuffer buf;
        static AssetMod* selectedMod = nullptr;
        std::optional<size_t> selectedModIdx;
        if (selectedMod)
        {
            bool isSelectedModStillAlive = false;
            for (size_t i = 0; i < m_Mods.size(); i++)
            {
                auto& mod = m_Mods[i];
                if (mod.get() == selectedMod)
                {
                    isSelectedModStillAlive = true;
                    selectedModIdx = i;
                    break;
                }
            }
            if (!isSelectedModStillAlive)
                selectedMod = nullptr;
        }

        const float leftPaneHeight = ImGui::GetTextLineHeightWithSpacing() * 20;
        const float leftPaneWidth = -ImGui::GetWindowWidth() * 0.6f;
        ImGuiChildFlags leftPaneFlags =
            ImGuiChildFlags_ResizeX
            | ImGuiChildFlags_Borders
            ;
        if (ImGuiCTX::WindowChild _sectionMods{ "_sectionMods", ImVec2(leftPaneWidth, leftPaneHeight), leftPaneFlags })
        {
            std::optional<size_t> idxDragged = {};
            std::optional<size_t> idxHovered = {};
            for (size_t n = 0; n < m_Mods.size(); n++)
            {
                auto& mod = m_Mods[n];
                buf.clear();
                const bool isActivated = mod->m_IsActiveInLoadOrder;
                buf.appendf(
                    "%2d. %s %s %s###%s"
                    , n
                    , isActivated ? "[+]" : "   "
                    , mod->m_ModName.c_str()
                    , mod->m_WasLoadedFromLoadOrderButNotFoundAnymore ? "(MISSING)" : ""
                    , mod->m_ModName.c_str()
                );
                const bool isSelected = mod.get() == selectedMod;
                const bool isModInvalid = mod->m_IsError;
                const bool isInConflictWithSelected = doShowConflicts && !isSelected && selectedMod && IsModHasConflicts(*mod, *selectedMod);
                const bool isColoredSelectable = isSelected || isInConflictWithSelected;
                std::optional<ImVec4> textColor = {};
                std::optional<ImVec4> bgColor = {};
                ImVec4 color_inConflictLowerPriority(0.396f, 0.859f, 0.517f, 0.188f);
                ImVec4 color_inConflictHigherPriority(0.980f, 0.350f, 0.260f, 0.188f);
                if (isInConflictWithSelected)
                    bgColor = (n < *selectedModIdx) ? color_inConflictLowerPriority : color_inConflictHigherPriority;
                if (isModInvalid) textColor = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                else if (mod->m_Datapacks.size() == 0 && mod->m_LooseFiles.size() == 0) textColor = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
                if (textColor) ImGui::PushStyleColor(ImGuiCol_Text, *textColor);
                if (bgColor) ImGui::PushStyleColor(ImGuiCol_Header, *bgColor);
                const bool isClicked = ImGui::Selectable(buf.c_str(), isColoredSelectable);
                if (bgColor) ImGui::PopStyleColor();
                if (textColor) ImGui::PopStyleColor();
                const bool isActive = ImGui::IsItemActive();
                const bool isHoveredRect = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly);
                const bool isRightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
                if (isActive)       idxDragged = n;
                if (isHoveredRect)  idxHovered = n;
                if (isClicked)      { selectedMod = mod.get(); selectedModIdx = n; }
                if (isRightClicked) ToggleMod(*mod);
            }
            if (idxDragged && idxHovered && *idxDragged != *idxHovered)
            {
                const bool isMovingUp = *idxDragged < *idxHovered;
                int direction = isMovingUp ? +1 : -1;
                for (size_t i = *idxDragged; i != *idxHovered; i += direction)
                {
                    std::swap(m_Mods[i], m_Mods[i + direction]);
                }
                ApplyCustomForgeOrder();
            }
        }
        ImGui::SameLine();

        if (ImGuiCTX::WindowChild _sectionSelectedModContents{ "_sectionSelectedModContents", ImVec2(0, leftPaneHeight), false })
        {
            DrawPopup_DatapackDetails();
            if (selectedMod)
            {
                auto itSelectedMod = std::find_if(m_Mods.begin(), m_Mods.end(), [](auto& mod) { return mod.get() == selectedMod; });
                if (itSelectedMod == m_Mods.end())
                {
                    selectedMod = nullptr;
                }
                else
                {
                    bool doDelete = false;
                    auto& mod = *itSelectedMod;
                    ImGui::Text(
                        "[%s]: %d datapacks, %u loose files."
                        , mod->m_ModName.c_str()
                        , mod->m_Datapacks.size()
                        , mod->m_LooseFiles.size()
                    );
                    if (mod->m_WasLoadedFromLoadOrderButNotFoundAnymore)
                    {
                        if (ImGui::SmallButton("Delete"))
                        {
                            doDelete = true;
                        }
                        ImGui::SameLine();
                        ImGui::Text("Mod was saved in load order but is no longer present.");
                    }
                    if (ImGuiCTX::TabBar _tbModContents{ "tbModContents" })
                    {
                        ImVec4 color_tabError = ImVec4(0.743f, 0.308f, 0.308f, 0.700f);
                        ImVec4 color_tabErrorHovered = ImVec4(0.980f, 0.406f, 0.406f, 0.862f);
                        ImVec4 color_tabErrorSelected = ImVec4(0.743f, 0.308f, 0.308f, 1.000f);
                        ImVec4 color_tabEmpty = ImVec4(0.263f, 0.305f, 0.361f, 0.863f);
                        ImVec4 color_tabEmptyHovered = ImVec4(0.414f, 0.499f, 0.604f, 0.800f);
                        ImVec4 color_tabEmptySelected = ImVec4(0.298f, 0.368f, 0.455f, 1.000f);
                        const bool isNoDatapacks = mod->m_Datapacks.size() == 0;
                        const bool isNoLooseFiles = mod->m_LooseFiles.size() == 0;
                        bool isColoredTab_Datapacks = false;
                        bool isColoredTab_LooseFiles = false;
                        if (mod->m_IsErrorInDatapacks)
                        {
                            ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.743f, 0.308f, 0.308f, 0.700f));
                            ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.980f, 0.406f, 0.406f, 0.862f));
                            ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.743f, 0.308f, 0.308f, 1.000f));
                            isColoredTab_Datapacks = true;
                        }
                        else if (isNoDatapacks)
                        {
                            //ImGui::PushStyleColor(ImGuiCol_Tab, color_tabEmpty);
                            //ImGui::PushStyleColor(ImGuiCol_TabHovered, color_tabEmptyHovered);
                            //ImGui::PushStyleColor(ImGuiCol_TabActive, color_tabEmptySelected);
                            //isColoredTab_Datapacks = true;
                        }
                        buf.clear();
                        buf.appendf("Datapacks: %2u###Datapacks", mod->m_Datapacks.size());
                        if (ImGuiCTX::Tab _tabDatapacks{ buf.c_str()})
                        {

                    ImGui::Separator();
                    {
                        ImGuiTableFlags flags
                            = ImGuiTableFlags_RowBg
                            | ImGuiTableFlags_Borders
                            | ImGuiTableFlags_Resizable
                            ;
                        if (ImGui::BeginTable("##tableDatapacksInMod", 3, flags))
                        {
                            ImGui::TableSetupColumn("Name");
                            ImGui::TableSetupColumn("Handle");
                            ImGui::TableSetupColumn("Refs");
                            ImGui::TableHeadersRow();

                            for (size_t i = 0; i < mod->m_Datapacks.size(); i++)
                            {
                                auto& datapack = mod->m_Datapacks[i];
                                auto* datapackOk = std::get_if<ResultForDetectedDatapack_LooksOk>(&datapack->m_Result);
                                auto* datapackInvalid = std::get_if<ResultForDetectedDatapack_LooksInvalid>(&datapack->m_Result);
                                auto* datapackFailedToLoad = std::get_if<ResultForDetectedDatapack_LookedOkButFailedToCreateVirtualForge>(&datapack->m_Result);
                                ImGui::TableNextRow();
                                const bool isDatapackInvalid = datapackOk == nullptr;
                                AssetMod* overridingMod = nullptr;
                                if (doShowConflicts && selectedMod->m_IsActiveInLoadOrder)
                                    overridingMod = FindHigherPriorityOverride(*datapack);
                                const bool isOverridden = overridingMod != nullptr;
                                const bool isColoredSelectable = isOverridden;
                                ImVec4 color_inConflictHigherPriority(0.980f, 0.350f, 0.260f, 0.188f);

                                ImGui::TableSetColumnIndex(0);

                                if (isDatapackInvalid) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
                                if (isColoredSelectable) ImGui::PushStyleColor(ImGuiCol_Header, color_inConflictHigherPriority);
                                const bool isDatapackClicked = ImGui::Selectable((const char*)datapack->m_AbsolutePath.filename().u8string().c_str(), isColoredSelectable);
                                if (isColoredSelectable) ImGui::PopStyleColor();
                                if (isDatapackClicked)
                                {
                                    OpenPopup_DatapackDetails(*selectedMod, *datapack);
                                }
                                if (isDatapackInvalid) ImGui::PopStyleColor();
                                if (ImGui::IsItemHovered())
                                {
                                    if (datapackOk)
                                    {
                                        if (isOverridden)
                                            ImGui::SetTooltip("Is overridden by [%s]", overridingMod->m_ModName.c_str());
                                    }
                                    else if (datapackInvalid)
                                    {
                                        ImGui::SetTooltip(
                                            "Does not look like a valid datapack:\n%s\n"
                                            "The game does not check for errors and can freeze, so I'm not loading this mod."
                                            , datapackInvalid->m_ErrorMsg.c_str()
                                        );
                                    }
                                    else if (datapackFailedToLoad)
                                    {
                                        ImGui::SetTooltip((const char*)datapackFailedToLoad->m_ErrorMsg.c_str());
                                    }
                                }
                                if (datapackOk)
                                {
                                    ImGui::TableSetColumnIndex(1);
                                    char buf[32];
                                    sprintf_s(buf, std::size(buf), "%llu", datapackOk->m_Handle);
                                    ImGui::Text(buf);
                                    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) ImGui::SetClipboardText(buf);
                                    ImGui::TableSetColumnIndex(2);
                                    ACU::WeakRef<ManagedObject> weakRef(datapackOk->m_Handle);
                                    const bool isLoaded = weakRef.GetSharedBlock().GetPtr();
                                    const uint32 strongRefs = weakRef.GetSharedBlock().GetRefcountStrong();
                                    if (isLoaded)
                                    {
                                        ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.4f, 0.8f, 0.4f, 0.65f));
                                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
                                    }
                                    ImGui::Text("%u", strongRefs);
                                }
                            }
                            ImGui::EndTable();
                        }
                    }
                        }
                        if (isColoredTab_Datapacks)
                            ImGui::PopStyleColor(3);

                        if (mod->m_IsErrorInLooseFiles)
                        {
                            ImGui::PushStyleColor(ImGuiCol_Tab, color_tabError);
                            ImGui::PushStyleColor(ImGuiCol_TabHovered, color_tabErrorHovered);
                            ImGui::PushStyleColor(ImGuiCol_TabActive, color_tabErrorSelected);
                            isColoredTab_LooseFiles = true;
                        }
                        else if (isNoLooseFiles)
                        {
                            //ImGui::PushStyleColor(ImGuiCol_Tab, color_tabEmpty);
                            //ImGui::PushStyleColor(ImGuiCol_TabHovered, color_tabEmptyHovered);
                            //ImGui::PushStyleColor(ImGuiCol_TabActive, color_tabEmptySelected);
                            //isColoredTab_LooseFiles = true;
                        }
                        buf.clear();
                        buf.appendf("Loose files: %2u###Loose files", mod->m_LooseFiles.size());
                        if (ImGuiCTX::Tab _tabLooseFiles{ buf.c_str()})
                        {
                            ImGuiTableFlags flags
                                = ImGuiTableFlags_RowBg
                                | ImGuiTableFlags_Borders
                                | ImGuiTableFlags_Resizable
                                ;
                            if (ImGui::BeginTable("##tableDatapacksInMod", 3, flags))
                            {
                                ImGui::TableSetupColumn("Name");
                                ImGui::TableSetupColumn("Handle");
                                ImGui::TableSetupColumn("Refs");
                                ImGui::TableHeadersRow();

                                for (size_t i = 0; i < mod->m_LooseFiles.size(); i++)
                                {
                                    auto& looseFile = mod->m_LooseFiles[i];
                                    auto* stateOk = std::get_if<ResultForDetectedLooseFile_LooksOk>(&looseFile->m_Result);
                                    ImGui::TableNextRow();
                                    const bool isInvalid = stateOk == nullptr;
                                    ImGui::TableSetColumnIndex(0);
                                    if (isInvalid) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
                                    ImGui::Text((const char*)looseFile->m_AbsolutePath.filename().u8string().c_str());
                                    if (isInvalid) ImGui::PopStyleColor();

                                    if (ImGui::IsItemHovered())
                                    {
                                        auto* errorMsg = looseFile->GetError();
                                        if (errorMsg)
                                        {
                                            ImGui::SetTooltip((const char*)errorMsg->c_str());
                                        }
                                    }
                                    if (stateOk)
                                    {
                                        ImGui::TableSetColumnIndex(1);
                                        char buf[32];
                                        sprintf_s(buf, std::size(buf), "%llu", stateOk->m_Handle);
                                        ImGui::Text(buf);
                                        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) ImGui::SetClipboardText(buf);
                                        ImGui::TableSetColumnIndex(2);
                                        ACU::WeakRef<ManagedObject> weakRef(stateOk->m_Handle);
                                        const bool isLoaded = weakRef.GetSharedBlock().GetPtr();
                                        const uint32 strongRefs = weakRef.GetSharedBlock().GetRefcountStrong();
                                        if (isLoaded)
                                        {
                                            ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.4f, 0.8f, 0.4f, 0.65f));
                                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
                                        }
                                        ImGui::Text("%u", strongRefs);
                                    }
                                }
                                ImGui::EndTable();
                            }
                        }
                        if (isColoredTab_LooseFiles)
                            ImGui::PopStyleColor(3);
                    }
                    if (doDelete)
                    {
                        m_Mods.erase(std::remove(m_Mods.begin(), m_Mods.end(), mod));
                    }
                }
            }
        }
    }
    json::JSON LoadOrderToJSON()
    {
        JSON jsLoadOrderArray = JSON::Make(JSON::Class::Array);
        for (auto& mod : m_Mods)
        {
            fs::path relPath = fs::relative(mod->m_ModFolder, GetAssetModsFolder());
            JSON jsModRecord = JSON::Make(JSON::Class::Object);
            jsModRecord["Name"] = FilepathAdapter(relPath).ToJSON();
            jsModRecord["Enable"] = mod->m_IsActiveInLoadOrder;
            jsLoadOrderArray.append(std::move(jsModRecord));
        }
        return jsLoadOrderArray;
    }
    fs::path GetLoadOrderFilepath()
    {
        return AbsolutePathInThisDLLDirectory("AssetOverrides") / "load_order.json";
    }
    void LoadOrder_Save()
    {
        JSON jsLoadOrder = LoadOrderToJSON();
        LOG_DEBUG(VirtualForgesLog
            , "Save load order:\n%s"
            , jsLoadOrder.dump().c_str()
        );
        json::ToFile(jsLoadOrder, GetLoadOrderFilepath());
    }
    void Clear()
    {
        m_Mods.clear();
    }
    fs::path GetAssetModsFolder()
    {
        return AbsolutePathInThisDLLDirectory("AssetOverrides");
    }
    void InitFromLoadOrder()
    {
        fs::path assetModsFolder = GetAssetModsFolder();
        m_Mods.clear();

        const bool assetModsDirExists = fs::is_directory(assetModsFolder);
        if (!assetModsDirExists)
        {
            LOG_DEBUG(VirtualForgesLog,
                "Asset mods folder not found:\n%s"
                , assetModsFolder.u8string().c_str()
            );
            return;
        }
        JSON jsLoadOrder = json::FromFile(GetLoadOrderFilepath());
        LOG_DEBUG(VirtualForgesLog
            , "Read load order:\n%s\n"
            , jsLoadOrder.dump().c_str()
        );
        JSON& jsLoadOrderArray = jsLoadOrder;
        if (jsLoadOrderArray.JSONType() == JSON::Class::Array)
        {
            for (JSON& jsMod : jsLoadOrderArray.ArrayRange())
            {
                fs::path Name;
                if (!READ_JSON_VARIABLE(jsMod, Name, FilepathAdapter)) continue;
                if (Name.empty()) continue;
                bool Enable = false;
                READ_JSON_VARIABLE(jsMod, Enable, BooleanAdapter);
                auto& newMod = m_Mods.emplace_back(std::make_unique<AssetMod>(assetModsFolder / Name));
                newMod->m_IsActiveInLoadOrder = Enable;
                newMod->m_WasLoadedFromLoadOrderButNotFoundAnymore = true;
            }
        }
        try {
            for (const auto& entry : fs::directory_iterator(assetModsFolder)) {
                if (!entry.is_directory()) continue;
                LOG_DEBUG(VirtualForgesLog
                    , "Found mod folder: %s\n"
                    , entry.path().filename().u8string().c_str()
                );
                auto itAlreadyPresent = std::find_if(m_Mods.begin(), m_Mods.end(), [&entry](std::unique_ptr<AssetMod>& mod)
                    {
                        return mod->m_ModFolder == entry;
                    });
                std::unique_ptr<AssetMod>& mod = (itAlreadyPresent == m_Mods.end())
                    ? m_Mods.emplace_back(std::make_unique<AssetMod>(entry))
                    : *itAlreadyPresent
                    ;
                mod->GatherDatapacksAndLooseFiles();
                mod->m_WasLoadedFromLoadOrderButNotFoundAnymore = false;
            }
        }
        catch (const fs::filesystem_error& e) {
            LOG_DEBUG(
                VirtualForgesLog
                , "[AddVirtualForges] Filesystem error: %s\n"
                , e.what()
            );
        }
        MakeVirtualForgesForAllDatapacks();
        ApplyCustomForgeOrder();
        RebuildLooseFilesLookup();
    }
    std::vector<DetectedDatapack*> GatherDatapackOrder()
    {
        std::vector<DetectedDatapack*> gatheredLoadOrder;
        for (auto& mod : m_Mods)
        {
            if (!mod->m_IsActiveInLoadOrder
                || mod->m_IsError
                || mod->m_Datapacks.size() == 0
                )
                continue;
            for (auto& datapack : mod->m_Datapacks)
            {
                gatheredLoadOrder.push_back(datapack.get());
            }
        }
        return gatheredLoadOrder;
    }
    void MakeVirtualForgesForAllDatapacks()
    {
        auto* fm = ForgeManager::GetSingleton();
        if (!fm) return;
        auto MakeForgeForDatapack = [fm](DetectedDatapack& datapack) -> bool
            {
                if (datapack.m_ForgeIdx && IsForgeAlive(*fm, *datapack.m_ForgeIdx))
                    return true;
                const uint64 handle = std::get<ResultForDetectedDatapack_LooksOk>(datapack.m_Result).m_Handle;
                ForgeFileEntry* newForgeEntry = MakeNewForgeFileEntry(handle, datapack.m_AbsolutePath, LoadPriority::Highest);
                if (!newForgeEntry) return false;
                datapack.m_ForgeIdx = newForgeEntry->forgeIdx_mb;
                return true;
            };
        for (auto& mod : m_Mods)
        {
            if (!mod->m_IsActiveInLoadOrder
                || mod->m_IsError
                || mod->m_Datapacks.size() == 0
                )
                continue;
            for (auto& datapack : mod->m_Datapacks)
            {
                bool isSuccess = MakeForgeForDatapack(*datapack);
                if (!isSuccess)
                {
                    datapack->m_Result = ResultForDetectedDatapack_LookedOkButFailedToCreateVirtualForge{
                        u8"Datapack looked OK when it was found, but the game failed to load it.\n"
                        "Was the file removed? Try refreshing the Load Order."
                    };
                    mod->m_IsErrorInDatapacks = true;
                    mod->m_IsError = true;
                    break;
                }
            }
        }
    }
    void UpdatePrefetchDataForActiveDatapacks()
    {
        auto* fm = ForgeManager::GetSingleton();
        if (!fm) return;
        auto MakeForgeForDatapack = [fm](DetectedDatapack& datapack) -> bool
            {
                if (datapack.m_ForgeIdx && IsForgeAlive(*fm, *datapack.m_ForgeIdx))
                    return true;
                const uint64 handle = std::get<ResultForDetectedDatapack_LooksOk>(datapack.m_Result).m_Handle;
                ForgeFileEntry* newForgeEntry = MakeNewForgeFileEntry(handle, datapack.m_AbsolutePath, LoadPriority::Highest);
                if (!newForgeEntry) return false;
                datapack.m_ForgeIdx = newForgeEntry->forgeIdx_mb;
                return true;
            };
        for (auto& mod : m_Mods)
        {
            if (!mod->m_IsActiveInLoadOrder
                || mod->m_IsError
                || mod->m_Datapacks.size() == 0
                )
                continue;
            for (auto& datapack : mod->m_Datapacks)
            {
                if (!datapack->m_ForgeIdx) continue;
                const uint64 handle = std::get<ResultForDetectedDatapack_LooksOk>(datapack->m_Result).m_Handle;
                auto FindForgeForIdx = [&](ForgeIndex_t forgeIdx) -> ForgeFileEntry*
                    {
                        for (ForgeFileEntry* forgeEntry : fm->forges)
                        {
                            if (forgeEntry->forgeIdx_mb == forgeIdx) return forgeEntry;
                        }
                        return nullptr;
                    };
                ForgeFileEntry* moddedForge = FindForgeForIdx(*datapack->m_ForgeIdx);
                if (!moddedForge) continue;
                auto [nonmoddedForge, prefetchInfoIdx] = this->FindOriginalPrefetchingInfoForHandle(handle);
                if (nonmoddedForge)
                    StealPrefetchingInfoIntoVirtualForge(*moddedForge->forgeContentsDescriptor, *nonmoddedForge->forgeContentsDescriptor, prefetchInfoIdx);
                else
                    ResetPrefetchingInfoForVirtualForge(*moddedForge->forgeContentsDescriptor, handle);
            }
        }
    }
    void ToggleMod(AssetMod& mod, bool doActivate)
    {
        const bool wasActive = !doActivate;
        mod.m_IsActiveInLoadOrder = !mod.m_IsActiveInLoadOrder;
        auto* fm = ForgeManager::GetSingleton();
        if (wasActive)
        {
            for (auto& datapack : mod.m_Datapacks)
            {
                if (datapack->m_ForgeIdx && IsForgeAlive(*fm, *datapack->m_ForgeIdx))
                {
                    ForgeManager__DecrementForgeEntryRefcount_mb(fm, *datapack->m_ForgeIdx);
                    datapack->m_ForgeIdx.reset();
                }
            }
        }
        else
        {
            MakeVirtualForgesForAllDatapacks();
            ApplyCustomForgeOrder();
        }
        RebuildLooseFilesLookup();
    }
    void ToggleMod(AssetMod& mod)
    {
        ToggleMod(mod, !mod.m_IsActiveInLoadOrder);
    }
    void ApplyCustomForgeOrder()
    {
        auto* fm = ForgeManager::GetSingleton();
        if (!fm) return;
        ApplyCustomForgeOrder(*fm);
    }
    void ApplyCustomForgeOrder(ForgeManager& fm)
    {
        std::vector<DetectedDatapack*> gatheredLoadOrder = GatherDatapackOrder();
        // Move the corresponding forge entries to the front, sort them accordingly.
        std::vector<ForgeFileEntry*> orderedForgeEntries;
        orderedForgeEntries.reserve(gatheredLoadOrder.size());
        for (DetectedDatapack* datapack : gatheredLoadOrder)
        {
            if (!datapack->m_ForgeIdx) continue;
            for (ForgeFileEntry* forgeEntry : fm.forges)
            {
                if (forgeEntry->forgeIdx_mb == *datapack->m_ForgeIdx)
                    orderedForgeEntries.push_back(forgeEntry);
            }
        }
        auto firstRemovedIt = std::remove_if(fm.forges.begin(), fm.forges.end(), [&](ForgeFileEntry* forgeEntry)
            {
                return std::find(orderedForgeEntries.begin(), orderedForgeEntries.end(), forgeEntry) != orderedForgeEntries.end();
            });
        const uint16 numRemovedEntries = (fm.forges.end() - firstRemovedIt);
        if (numRemovedEntries)
        {
            fm.forges.size = fm.forges.size - numRemovedEntries;
        }
        for (ForgeFileEntry* forgeEntry : orderedForgeEntries)
        {
            ACU::Memory::SmallArrayInsert(fm.forges, forgeEntry, 0);
        }
    }

    fs::path GetPathInAssetModsFolder(const fs::path& pathRel)
    {
        return AbsolutePathInThisDLLDirectory("AssetOverrides") / pathRel;
    }

    struct RegisteredSingleObjectOverride
    {
        AssetMod* m_Mod;
        DetectedLooseFile* m_LooseFile;
        fs::path m_AbsolutePath;
    };
    std::unordered_map<uint64, RegisteredSingleObjectOverride> m_SingleObjectOverride_FilepathRelByHandle;
    void RebuildLooseFilesLookup()
    {
        m_SingleObjectOverride_FilepathRelByHandle.clear();
        for (auto& assetMod : m_Mods)
        {
            if (assetMod->m_IsError) continue;
            if (!assetMod->m_IsActiveInLoadOrder) continue;
            for (auto& looseFile : assetMod->m_LooseFiles)
            {
                auto& scannedLooseFile = std::get<ResultForDetectedLooseFile_LooksOk>(looseFile->m_Result);
                m_SingleObjectOverride_FilepathRelByHandle[scannedLooseFile.m_Handle] =
                    RegisteredSingleObjectOverride{ assetMod.get(), looseFile.get(), looseFile->m_AbsolutePath };
            }
        }
    }
    struct ResultOfSingleObjectRequest
    {
        AssetMod* m_Mod;
        DetectedLooseFile* m_LooseFile;
        fs::path m_AbsolutePath;
        std::vector<byte> m_FileAsBytes;

        bool IsSuccessfullyRead() { return m_FileAsBytes.size() != 0; }
    };
    ResultOfSingleObjectRequest GetSingleObjectOverrideFilepath(uint64 handle);
    void ReportFailureInSingleObjectOverride(const RegisteredSingleObjectOverride& whichFailed, const std::u8string& errorMsg);
};
AssetModlist g_AssetModlist;


void AssetOverrides_PutForgesInCorrectOrder()
{
    auto* fm = ForgeManager::GetSingleton();
    g_AssetModlist.MakeVirtualForgesForAllDatapacks();
    g_AssetModlist.ApplyCustomForgeOrder(*fm);
}
void WhenNewForgeEntryWasJustAdded_ApplyCustomSorting(AllRegisters* params)
{
    DEFINE_GAME_FUNCTION(ACU_enterCriticalSection, 0x14211F620, void, __fastcall, (uint64*));
    DEFINE_GAME_FUNCTION(ACU_leaveCriticalSection, 0x14212CBC0, void, __fastcall, (uint64*));
    auto* fm = ForgeManager::GetSingleton();
    ACU_enterCriticalSection(&fm->criticalSection_forgeEntries);
    g_AssetModlist.MakeVirtualForgesForAllDatapacks();
    g_AssetModlist.ApplyCustomForgeOrder(*fm);
    g_AssetModlist.UpdatePrefetchDataForActiveDatapacks();
    ACU_leaveCriticalSection(&fm->criticalSection_forgeEntries);
}

void DrawAssetOverridesSettings()
{
    g_AssetModlist.DrawMenu();
}


void WhenGatheringPrefetchInfoForDatapack_FindOriginalPrefetchInfo(AllRegisters* params)
{
    auto* fm = ForgeManager::GetSingleton();
    std::vector<DetectedDatapack*> activeModdedDatapacks = g_AssetModlist.GatherDatapackOrder();

    ForgeFile* forgeContents = (ForgeFile*)params->r11_;
    uint64 handle = (uint64&)params->rdi_;
    auto FindCorrespondingForgeIfItsOneOfMine = [&]() -> ForgeFileEntry*
        {
            for (ForgeFileEntry* forge : fm->forges)
            {
                if (forge->forgeContentsDescriptor != forgeContents) continue;
                for (DetectedDatapack* moddedDatapack : activeModdedDatapacks)
                {
                    if (moddedDatapack->m_ForgeIdx && *moddedDatapack->m_ForgeIdx == forge->forgeIdx_mb)
                    {
                        LOG_DEBUG(VirtualForgesLog
                            , "Trying to load a modded datapack:\n    %s\n"
                            , moddedDatapack->m_AbsolutePath.u8string().c_str()
                        );
                        return forge;
                    }
                }
            }
            return nullptr;
        };
    ForgeFileEntry* correspondingForge = FindCorrespondingForgeIfItsOneOfMine();
    const bool isOneOfMyVirtualForges = correspondingForge != nullptr;
    if (!isOneOfMyVirtualForges) return;
    auto [nonmoddedForge, prefetchInfoIdx] = g_AssetModlist.FindOriginalPrefetchingInfoForHandle(handle);
    if (!nonmoddedForge) return;
    StealPrefetchingInfoIntoVirtualForge(*forgeContents, *nonmoddedForge->forgeContentsDescriptor, prefetchInfoIdx);
}
void DrawAssetOverridesInstructions()
{
    static ImVec4 colorAttentionHeader(0.847f, 0.337f, 0.323f, 1.0f);
    //ImGui::ColorEdit3("Header col", &colorAttentionHeader.x);
    ImGui::PushStyleColor(ImGuiCol_Header,          ImVec4(colorAttentionHeader.x, colorAttentionHeader.y, colorAttentionHeader.z, 0.55f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,   ImVec4(colorAttentionHeader.x, colorAttentionHeader.y, colorAttentionHeader.z, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,    ImVec4(colorAttentionHeader.x, colorAttentionHeader.y, colorAttentionHeader.z, 1.00f));
    if (ImGuiCTX::TreeNodeEx _virtualForgesReadme{ "EXPERIMENTAL - PLEASE READ", ImGuiTreeNodeFlags_Framed })
    {
        ImGui::PopStyleColor(3);
        ImGui::Text(
            "Some of the mods that you would normally install via the AnvilToolkit"
            "\ncan instead be loaded at runtime."
            "\nIf you, like me, are reluctant to backup, unpack and repack the big .forge files"
            "\njust to try out a new outfit, then this feature might suffice."
            "\nThe game does not check for errors, so please read carefully"
            "\nand _do_ make a backup of your savegame."
        );
        if (ImGuiCTX::TreeNode _exampleUsage{ "Example: How to install a new outfit" })
        {
            //if (ImGuiCTX::TreeNode _tldr{ "TL;DR:" })
            //{
            //    ImGui::Text(
            //        "Typical usage: how to install a new outfit replacer.\n"
            //        "Put the outfit's \".data\" file into\n"
            //        "       \"Assassin's Creed Unity/ACUFixes/plugins/ACUFixes/AssetOverrides/YOUR_MOD_NAME/\",\n"
            //        "then click \"Refresh load order\", then right click \"YOUR_MOD_NAME\" to activate.\n"
            //        "Please read the rest of the instructions."
            //    );
            //}
            ImGui::Text(
                "Typical usage: installing a new outfit replacer.\n"
                "I'll use the \"Assassins Creed Victory Outfit\" v1.0 for AC Unity as an example.\n"
                "1. Download the mod.\n"
                "2. From the downloaded .zip file, extract the\n"
                "       \"1_-_CN_P_FR_LegacyAvatar_Altair.data\"\n"
                "       (you can rename the file, but the datapack needs to have extension \".data\")\n"
                "   into\n"
                "       \"Assassin's Creed Unity/ACUFixes/plugins/AssetOverrides/VictoryOutfit/\"\n"
                "       (if your mod has multiple .data files\n"
                "       (for example the \"Templar Extremist Retexture + Remove Distance Glow\" by MaceoniK\n"
                "       has 30 .data files), put them all in the same folder.)\n"
                "3. In game, in the Asset Overrides menu, in the Load Order section,\n"
                "   click \"Refresh load order\"\n"
                "   You should now see\n"
                "       0.     VictoryOutfit\n"
                "4. Right click to enable it.\n"
                "   You should now see\n"
                "       0. [+] VictoryOutfit\n"
                "5. If you now equip the Altair Outfit from the Customization Menu->Outfits,\n"
                "   you should see the Victory Outfit instead.\n"
                "6. Now please, do read the rest of the instructions."
            );
        }
        if (ImGuiCTX::TreeNode _limitations{ "Limitations:" })
        {
            ImGui::BulletText(
                "This is not a replacement for AnvilToolkit, merely a convenience feature\n"
                "to avoid unpacking/repacking large .forge files in _some_ cases.\n"
                "It is inconclusive for now whether or not the very act of swapping assets\n"
                "at runtime leads to more crashes.\n"
                "If you're experiencing crashes, you can install your mods\n"
                "The usual way via the AnvilToolkit\n"
                "which will always give you the most stable results.\n"
                "If you're suspecting that some crashes are caused by this feature,\n"
                "then just remove the\n"
                "    \"Assassin's Creed Unity/ACUFixes/plugins/ACUFixes/AssetOverrides/\"\n"
                "folder to completely disable all Asset Overrides."
            );
            ImGui::BulletText(
                "I cannot override some datapacks that are loaded very early.\n"
                "For example, I cannot override the\n"
                "    \"110_-_Game Bootstrap Settings.data\" from \"DataPC.forge\"\n"
                "You'll have to use the AnvilToolkit for these.\n"
            );
            ImGui::BulletText(
                "If the game object that you're trying to replace is _currently_being_used_\n"
                "then it will not be overridden until it _stops_being_used_ and is unloaded first.\n"
                "Going back to the VictoryOutfit example:\n"
                "1. Start the game normally, don't enable any of the asset overrides.\n"
                "2. Equip the Altair Outfit (the unmodded one).\n"
                "3. Enable the VictoryOutfit in the load order.\n"
                "4. The Altair Outfit remains unchanged. It will remain unchanged\n"
                "   at least until you unequip it and resume the game.\n"
                "   More precisely, it will remain unchanged until all usages/references\n"
                "   of it are cleared and the datapack is unloaded.\n"
                "   Only then will the modded outfit be able to load.\n"
                "If you open the Load Order section and click on a mod, then on the right pane\n"
                "you'll be able to see the detected datapacks within the mod, as well as their handles\n"
                "    (150860920202 in case of Altair Outfit and Victory Outfit,\n"
                "    which is the handle of\n"
                "    \"CN_P_FR_LegacyAvatar_Altair\\CN_P_FR_LegacyAvatar_Altair.EntityBuilder\")\n"
                "and the number of references.\n"
                "When the number of references is green, it means that the datapack\n"
                "with this handle is currently in use.\n"
                "When the number of references reaches 0, the datapack will be unloaded,\n"
                "at which point the overrides can become active.\n"
                "Sometimes you'll need to restart the game so that the datapack can be freshly loaded\n"
                "(and again, some datapacks are loaded too early for me to override at all - see\n"
                "   \"Game Bootstrap Settings.data\")."
            );
            ImGui::BulletText(
                "Some AnvilToolkit mods are supposed to be installed in a particular World\n"
                "(repacked into DataPC_ACU_LGS_BelleEpoque.forge,\n"
                "DataPC_ACU_Paris.forge, DataPC_ACU_Versailles.forge etc.),\n"
                "but Asset Overrides are active in every World.\n"
                "This might be what you want, but can theoretically cause problems if mods are used\n"
                "in a way they weren't supposed to be.\n"
                "If you're experiencing problems, just disable Asset Overrides and try installing\n"
                "your mod the usual way via the AnvilToolkit."
            );
        }
        if (ImGuiCTX::TreeNode _knownMods{ "Compatible mods" })
        {
            ImGui::Text("Examples of mods I've confirmed to work when installed through Asset Overrides\n"
                "(right click to copy to clipboard):");
            auto ModTextAndLink = [](const char* modName, const char* link)
                {
                    ImGui::BulletText(modName);
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip(link);
                    }
                    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                    {
                        ImGui::SetClipboardText(link);
                    }
                };
            ImVec4 colorWarning(0.869f, 0.761f, 0.376f, 1.0f);
            ImVec4 colorIncompatible(0.869f, 0.389f, 0.376f, 1.0f);
            ModTextAndLink("Templar Extremist Retexture + Remove Distance Glow by MaceoniK", "https://www.nexusmods.com/assassinscreedunity/mods/133");
            ModTextAndLink(
                "Play As Elise by Halzoid98 (of the 3 .data files it's best to\n"
                "use only the one that goes in DataPC_ACU_Paris.forge.\n"
                "Also the face texture can be missing at first, but just\n"
                "switch to a different outfit and back - this seems to be a known issue.)", "https://www.nexusmods.com/assassinscreedunity/mods/39");
            ModTextAndLink("Elise in Lydia Frye's outfit by th3kill", "https://www.nexusmods.com/assassinscreedunity/mods/124");
            ModTextAndLink("Assassins Creed Victory Outfit v1.0 by LordOfThe9", "https://www.nexusmods.com/assassinscreedunity/mods/136");
            ModTextAndLink("Play as Ghost Face v1.00 by LeafyyIsHere", "https://www.nexusmods.com/assassinscreedunity/mods/271");
            ModTextAndLink("Big Smoke by Kuza49", "https://www.nexusmods.com/assassinscreedunity/mods/255");
            ModTextAndLink("ACU Original MOD Collection IV Female Character Outfit Port by nicholasllj", "https://www.nexusmods.com/assassinscreedunity/mods/228");
            ModTextAndLink("ACU play as young Arno by Petrichor23", "https://www.nexusmods.com/assassinscreedunity/mods/197");
            ModTextAndLink("Lucy Thorne Outfit by Kuza49", "https://www.nexusmods.com/assassinscreedunity/mods/198");
            ModTextAndLink("Parkour Front flip and Jump mod by Petrichor23", "https://www.nexusmods.com/assassinscreedunity/mods/222");

            ImGui::PushStyleColor(ImGuiCol_Text, colorWarning);
            ModTextAndLink(
                "Jacob's Outfits by th3kill (works OK, but seems to be\n"
                "prone to arbitrary crashes when used with other mods?\n"
                "Especially if quickly changing outfits in the menu.\n"
                "Not sure why. You can definitely still try them out)"
                , "https://www.nexusmods.com/assassinscreedunity/mods/217"
            );
            ModTextAndLink(
                "Eiffel Tower by LoremIpsum12 (known to crash during fast travel,\n"
                "but works otherwise)"
                , "https://www.nexusmods.com/assassinscreedunity/mods/225");
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Text, colorIncompatible);
            ImGui::BulletText(
                "Pretty much all weapon replacers, because they use \"Game Bootstrap Settings.data\"\n"
                "which is loaded too early for me to hook."
            );
            ImGui::PopStyleColor();
        }
    }
    else
        ImGui::PopStyleColor(3);
}
void AssetOverrides_InitFromLoadOrder()
{
    g_AssetModlist.InitFromLoadOrder();
}
void AssetOverrides_InitFromLoadOrder_EarlyHook()
{
    g_AssetModlist.InitFromLoadOrder();
    g_AssetModlist.UpdatePrefetchDataForActiveDatapacks();
    AssetOverrides_PutForgesInCorrectOrder();
}

// Thanks, https://stackoverflow.com/questions/15138353/how-to-read-a-binary-file-into-a-vector-of-unsigned-chars/21802936#21802936
static std::vector<byte> readFile(const fs::path& filename)
{
    std::vector<byte> vec;

    // open the file:
    std::ifstream file(filename, std::ios::binary);
    if (!file) return vec;

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
        std::istream_iterator<byte>(file),
        std::istream_iterator<byte>());

    return vec;
}
// Empty result means a failure.
std::vector<byte> ReadGameObjectFileWhole(const fs::path& filepath)
{
    std::vector<byte> fileAsBytes = readFile(filepath);
    // A quick last check that it still kinda looks like a valid game object file.
    if (fileAsBytes.size() <= 14)
    {
        fileAsBytes.clear();
        return fileAsBytes;
    }
    return fileAsBytes;
}
// Empty result means a failure.
AssetModlist::ResultOfSingleObjectRequest AssetModlist::GetSingleObjectOverrideFilepath(uint64 handle)
{
    if (!handle) return {};
    auto foundIt = m_SingleObjectOverride_FilepathRelByHandle.find(handle);
    if (foundIt == m_SingleObjectOverride_FilepathRelByHandle.end()) return {};
    RegisteredSingleObjectOverride registeredOverride = foundIt->second;
    std::vector<byte> modFileAsBytes;
    modFileAsBytes = ReadGameObjectFileWhole(registeredOverride.m_AbsolutePath);
    const bool failedToReadModfile = modFileAsBytes.size() == 0;
    if (failedToReadModfile)
    {
        ReportFailureInSingleObjectOverride(registeredOverride,
            u8"Game object Loose File looked OK when first found, but then failed to load.\n"
            "Was it removed? Try refreshing the Load Order."
        );
        return ResultOfSingleObjectRequest{ nullptr, nullptr, {}, {} };
    }

    return ResultOfSingleObjectRequest{
        registeredOverride.m_Mod,
        registeredOverride.m_LooseFile,
        std::move(registeredOverride.m_AbsolutePath),
        std::move(modFileAsBytes)
    };
}
void AssetModlist::ReportFailureInSingleObjectOverride(const RegisteredSingleObjectOverride& whichFailed, const std::u8string& errorMsg)
{
    whichFailed.m_LooseFile->m_Result = ResultForDetectedLooseFile_LookedOkButFailedToLoad{ errorMsg };
    whichFailed.m_Mod->m_IsErrorInLooseFiles = true;
    whichFailed.m_Mod->m_IsError = true;
}
std::vector<byte> GetSingleObjectOverrideBytes(uint64 handle)
{
    auto lookupResult = g_AssetModlist.GetSingleObjectOverrideFilepath(handle);
    if (!lookupResult.IsSuccessfullyRead()) return {};
    LOG_DEBUG(VirtualForgesLog,
        "FULLDESER: %s\n", lookupResult.m_AbsolutePath.u8string().c_str());
    return std::move(lookupResult.m_FileAsBytes);
}













void WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement(uint64 rax, uint64 rbx, uint64 rsi, uint64 rbp, uint64 rsp, uint64 r13);
void WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement(AllRegisters* params)
{
    WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement(params->GetRAX(), params->rbx_, params->rsi_, params->rbp_, params->GetRSP(), params->r13_);
}


class ManagedObject;
class DeserializationStream;
AddVirtualForges::AddVirtualForges()
{
    uintptr_t whenGameForgesJustOpened = 0x14272F794;
    PresetScript_CCodeInTheMiddle(whenGameForgesJustOpened, 7,
        WhenNewForgeEntryWasJustAdded_ApplyCustomSorting, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    uintptr_t whenStartDeserializeFileInDatapackTogetherWithPrologue = 0x1426F0CF7;
    PresetScript_CCodeInTheMiddle(whenStartDeserializeFileInDatapackTogetherWithPrologue, 5,
        WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement, 0x1426F0DC7, false);
}
void AddVirtualForges::OnBeforeActivate()
{
    AssetOverrides_InitFromLoadOrder();
}
void AddVirtualForges::OnBeforeDeactivate()
{
    g_AssetModlist.Clear();
}

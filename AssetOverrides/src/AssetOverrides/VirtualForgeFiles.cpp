#include "pch.h"

#include "ACU/ForgeManager.h"
#include "ACU/ManagedPtrs/ManagedPtrs.h"
#include "AssetOverrides.h"
#include "Common_Plugins/Common_PluginSide.h"
#include "Serialization/Serialization.h"
#include "Serialization/NumericAdapters.h"
#include "Serialization/BooleanAdapter.h"
#include "Serialization/FilepathAdapter.h"
#include "Serialization/ToFromFile/ToFromFile.h"

#include "Handles.h"

#include "ImGuiCTX.h"

#include "VirtualForgeFiles_impl.h"
#include "SingleObjectOverride.h"


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
        if (m_ForgeIdx && fm->IsForgeAlive(*m_ForgeIdx))
            fm->DecrementForgeRefcount(*m_ForgeIdx);
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


std::optional<uint16> GetPrefetchInfoIndex(ForgeFileEntry& forge, uint64 handle)
{
    auto& prefetchInfos = forge.forgeContentsDescriptor->datapacksPrefetchInfoAscendingHandles;
    if (!prefetchInfos.size) return {};
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
class AssetModlist
{
    std::vector<std::unique_ptr<AssetMod>> m_Mods;
public:
    std::vector<DetectedDatapack*> GatherDatapackOrder();
    void MakeVirtualForgesForAllDatapacks();
    void UpdatePrefetchDataForActiveDatapacks();
    void ApplyCustomForgeOrder();

    std::pair<ForgeFileEntry*, uint16> FindOriginalPrefetchingInfoForHandle(uint64 handle);
    void FindAndFormatOriginalPrefetchingInfoForHandle(ImGuiTextBuffer& buf, uint64 handle);
    struct Popup_DatapackDetails
    {
        bool isOpen = true;
        AssetMod* selectedMod = nullptr;
        DetectedDatapack* selectedDatapack = nullptr;
        ImGuiID ID{};
    } m_PDD;
    void OpenPopup_DatapackDetails(AssetMod& selectedMod, DetectedDatapack& datapack);
    void DrawPopup_DatapackDetails();
    void DrawMenu();

    AssetMod* FindHigherPriorityOverride(DetectedDatapack& datapack);

    json::JSON LoadOrderToJSON();
    fs::path GetAssetModsFolder()
    {
        return AbsolutePathInThisDLLDirectory("AssetOverrides");
    }
    fs::path GetPathInAssetModsFolder(const fs::path& pathRel)
    {
        return AbsolutePathInThisDLLDirectory("AssetOverrides") / pathRel;
    }
    fs::path GetLoadOrderFilepath()
    {
        return AbsolutePathInThisDLLDirectory("AssetOverrides") / "load_order.json";
    }

    void InitFromLoadOrder();
    void LoadOrder_Save();
    void Clear()
    {
        m_Mods.clear();
    }

    void ToggleMod(AssetMod& mod, bool doActivate);
    void ToggleMod(AssetMod& mod);


    struct RegisteredSingleObjectOverride
    {
        AssetMod* m_Mod;
        DetectedLooseFile* m_LooseFile;
        fs::path m_AbsolutePath;
    };
    struct ResultOfSingleObjectRequest
    {
        AssetMod* m_Mod;
        DetectedLooseFile* m_LooseFile;
        fs::path m_AbsolutePath;
        std::vector<byte> m_FileAsBytes;

        bool IsSuccessfullyRead() { return m_FileAsBytes.size() != 0; }
    };
    std::unordered_map<uint64, RegisteredSingleObjectOverride> m_SingleObjectOverride_FilepathRelByHandle;
    ResultOfSingleObjectRequest FindSingleObjectOverrideForHandle(uint64 handle);
    void RebuildLooseFilesLookup();
    void ReportFailureInSingleObjectOverride(const RegisteredSingleObjectOverride& whichFailed, const std::u8string& errorMsg);
};
AssetModlist g_AssetModlist;

std::pair<ForgeFileEntry*, uint16> AssetModlist::FindOriginalPrefetchingInfoForHandle(uint64 handle)
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
void AssetModlist::FindAndFormatOriginalPrefetchingInfoForHandle(ImGuiTextBuffer& buf, uint64 handle)
{
    auto [nonmoddedForge, prefetchInfoIdx] = FindOriginalPrefetchingInfoForHandle(handle);
    if (!nonmoddedForge)
    {
        buf.append("No prefetching info found in game's native forges.");
        return;
    }
    ForgeFile_DatapackPrefetchInfo& prefetchInfo = nonmoddedForge->forgeContentsDescriptor->datapacksPrefetchInfoAscendingHandles[prefetchInfoIdx];
    uintptr_t prefetchInfoAddr = (uintptr_t)nonmoddedForge->forgeContentsDescriptor->prefetchData + prefetchInfo.offsetInPrefetchData;
    std::string_view nonmoddedForgeName = nonmoddedForge->forgeContentsDescriptor->filename;
    if (!nonmoddedForgeName.size())
        nonmoddedForgeName = nonmoddedForge->forgeName;
    buf.appendf(
        "Found prefetching info in forge #%d (%s) at idx %hd (size=%u, isCompressed=%s, addr=%llX)"
        , nonmoddedForge->forgeIdx_mb
        , nonmoddedForgeName.data()
        , prefetchInfoIdx
        , prefetchInfo.prefetchDataSize
        , prefetchInfo.isCompressed_mb ? "true" : "false"
        , prefetchInfoAddr
    );
    return;
}
AssetMod* AssetModlist::FindHigherPriorityOverride(DetectedDatapack& datapack)
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
json::JSON AssetModlist::LoadOrderToJSON()
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
void AssetModlist::LoadOrder_Save()
{
    JSON jsLoadOrder = LoadOrderToJSON();
    LOG_DEBUG(VirtualForgesLog
        , "Save load order:\n%s"
        , jsLoadOrder.dump().c_str()
    );
    json::ToFile(jsLoadOrder, GetLoadOrderFilepath());
}
void AssetModlist::InitFromLoadOrder()
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
    UpdatePrefetchDataForActiveDatapacks();
    RebuildLooseFilesLookup();
}
std::vector<DetectedDatapack*> AssetModlist::GatherDatapackOrder()
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
void AssetModlist::MakeVirtualForgesForAllDatapacks()
{
    auto* fm = ForgeManager::GetSingleton();
    if (!fm) return;
    auto MakeForgeForDatapack = [fm](DetectedDatapack& datapack) -> bool
        {
            if (datapack.m_ForgeIdx && fm->IsForgeAlive(*datapack.m_ForgeIdx))
                return true;
            const uint64 handle = std::get<ResultForDetectedDatapack_LooksOk>(datapack.m_Result).m_Handle;
            ForgeFileEntry* newForgeEntry = MakeNewForgeFileEntry(handle, datapack.m_AbsolutePath);
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
void AssetModlist::UpdatePrefetchDataForActiveDatapacks()
{
    auto* fm = ForgeManager::GetSingleton();
    if (!fm) return;
    auto MakeForgeForDatapack = [fm](DetectedDatapack& datapack) -> bool
        {
            if (datapack.m_ForgeIdx && fm->IsForgeAlive(*datapack.m_ForgeIdx))
                return true;
            const uint64 handle = std::get<ResultForDetectedDatapack_LooksOk>(datapack.m_Result).m_Handle;
            ForgeFileEntry* newForgeEntry = MakeNewForgeFileEntry(handle, datapack.m_AbsolutePath);
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
            ForgeFileEntry* moddedForge = fm->FindForgeForIdx(*datapack->m_ForgeIdx);
            if (!moddedForge) continue;
            auto [nonmoddedForge, prefetchInfoIdx] = this->FindOriginalPrefetchingInfoForHandle(handle);
            if (nonmoddedForge)
                StealPrefetchingInfoIntoVirtualForge(*moddedForge->forgeContentsDescriptor, *nonmoddedForge->forgeContentsDescriptor, prefetchInfoIdx);
            else
                ResetPrefetchingInfoForVirtualForge(*moddedForge->forgeContentsDescriptor, handle);
        }
    }
}
void AssetModlist::ToggleMod(AssetMod& mod, bool doActivate)
{
    const bool wasActive = !doActivate;
    mod.m_IsActiveInLoadOrder = !mod.m_IsActiveInLoadOrder;
    auto* fm = ForgeManager::GetSingleton();
    if (wasActive)
    {
        for (auto& datapack : mod.m_Datapacks)
        {
            if (datapack->m_ForgeIdx && fm->IsForgeAlive(*datapack->m_ForgeIdx))
            {
                fm->DecrementForgeRefcount(*datapack->m_ForgeIdx);
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
void AssetModlist::ToggleMod(AssetMod& mod)
{
    ToggleMod(mod, !mod.m_IsActiveInLoadOrder);
}
void AssetModlist::ApplyCustomForgeOrder()
{
    auto& fm = *ForgeManager::GetSingleton();
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
void AssetModlist::RebuildLooseFilesLookup()
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

namespace ImGui
{
inline void CopyToClipboardOnClick(const char* s, const char* fmtTooltip = "Click to copy to clipboard", ...)
{
    va_list args;
    va_start(args, fmtTooltip);
    if (ImGui::IsItemHovered()) ImGui::SetTooltipV(fmtTooltip, args);
    va_end(args);
    if (ImGui::IsItemClicked()) ImGui::SetClipboardText(s);
}
}
void AssetModlist::OpenPopup_DatapackDetails(AssetMod& selectedMod, DetectedDatapack& datapack)
{
    m_PDD.isOpen = true;
    m_PDD.selectedMod = &selectedMod;
    m_PDD.selectedDatapack = &datapack;
    ImGui::OpenPopup(m_PDD.ID);
}
void AssetModlist::DrawPopup_DatapackDetails()
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
void AssetModlist::DrawMenu()
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
            "will be used at a time, so you might not face any issues.\n"
            "The opposite is also true: no detected conflicts does not necessarily mean\n"
            "that the mods will work together. It's safest to test new mods one by one."
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

    const float leftPaneHeight = -ImGui::GetTextLineHeightWithSpacing(); // ImGui::GetTextLineHeightWithSpacing() * 20;
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
            if (isClicked) { selectedMod = mod.get(); selectedModIdx = n; }
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
                    ImGuiCTX::PushStyleColor _colorModMissing(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
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
                    if (ImGuiCTX::Tab _tabDatapacks{ buf.c_str() })
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
                                {
                                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

                                    if (ImGui::TableSetColumnIndex(0))
                                        ImGui::TableHeader(ImGui::TableGetColumnName(0));
                                    //if (ImGui::IsItemHovered())
                                    //    ImGui::SetTooltip(
                                    //        "NAME"
                                    //    );
                                    if (ImGui::TableSetColumnIndex(1))
                                        ImGui::TableHeader(ImGui::TableGetColumnName(1));
                                    //if (ImGui::IsItemHovered())
                                    //    ImGui::SetTooltip(
                                    //        "HANDLE"
                                    //    );
                                    if (ImGui::TableSetColumnIndex(2))
                                        ImGui::TableHeader(ImGui::TableGetColumnName(2));
                                    if (ImGui::IsItemHovered())
                                        ImGui::SetTooltip(
                                            "Green means that the object with this handle is currently in use.\n"
                                            "You will not see any changes until it stops being used\n"
                                            "(number of refs reaches 0). Some object will not be unloaded\n"
                                            "until you restart the game."
                                        );
                                }

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
                    if (ImGuiCTX::Tab _tabLooseFiles{ buf.c_str() })
                    {
                        ImGuiTableFlags flags
                            = ImGuiTableFlags_RowBg
                            | ImGuiTableFlags_Borders
                            | ImGuiTableFlags_Resizable
                            ;
                        if (ImGui::BeginTable("##tableLooseFilesInMod", 3, flags))
                        {
                            ImGui::TableSetupColumn("Name");
                            ImGui::TableSetupColumn("Handle");
                            ImGui::TableSetupColumn("Refs");
                            {
                                ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

                                if (ImGui::TableSetColumnIndex(0))
                                    ImGui::TableHeader(ImGui::TableGetColumnName(0));
                                //if (ImGui::IsItemHovered())
                                //    ImGui::SetTooltip(
                                //        "NAME"
                                //    );
                                if (ImGui::TableSetColumnIndex(1))
                                    ImGui::TableHeader(ImGui::TableGetColumnName(1));
                                //if (ImGui::IsItemHovered())
                                //    ImGui::SetTooltip(
                                //        "HANDLE"
                                //    );
                                if (ImGui::TableSetColumnIndex(2))
                                    ImGui::TableHeader(ImGui::TableGetColumnName(2));
                                if (ImGui::IsItemHovered())
                                    ImGui::SetTooltip(
                                        "Green means that the object with this handle is currently in use.\n"
                                        "You will not see any changes until it stops being used\n"
                                        "(number of refs reaches 0). Some object will not be unloaded\n"
                                        "until you restart the game."
                                    );
                            }

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

// Empty result means a failure.
std::vector<byte> ReadGameObjectFileWhole(const fs::path& filepath);
// Empty result means a failure.
std::vector<byte> GetSingleObjectOverrideBytes(uint64 handle);
AssetModlist::ResultOfSingleObjectRequest AssetModlist::FindSingleObjectOverrideForHandle(uint64 handle)
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





void AssetOverrides_PutForgesInCorrectOrder()
{
    DEFINE_GAME_FUNCTION(ACU_enterCriticalSection, 0x14211F620, void, __fastcall, (uint64*));
    DEFINE_GAME_FUNCTION(ACU_leaveCriticalSection, 0x14212CBC0, void, __fastcall, (uint64*));
    auto* fm = ForgeManager::GetSingleton();
    ACU_enterCriticalSection(&fm->criticalSection_forgeEntries);
    g_AssetModlist.MakeVirtualForgesForAllDatapacks();
    g_AssetModlist.ApplyCustomForgeOrder();
    g_AssetModlist.UpdatePrefetchDataForActiveDatapacks();
    ACU_leaveCriticalSection(&fm->criticalSection_forgeEntries);
}

void DrawAssetOverridesSettings()
{
    g_AssetModlist.DrawMenu();
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
        auto ExampleModInstall = [](const auto& callblInAssetOverridesFolder) {
            if (ImGuiCTX::TreeNodeEx _{ "Assassins's Creed Unity/", ImGuiTreeNodeFlags_DefaultOpen }) {
                if (ImGuiCTX::TreeNodeEx _{ "ACUFixes/", ImGuiTreeNodeFlags_DefaultOpen }) {
                    if (ImGuiCTX::TreeNodeEx _{ "plugins/", ImGuiTreeNodeFlags_DefaultOpen }) {
                        if (ImGuiCTX::TreeNodeEx _{ "AssetOverrides/", ImGuiTreeNodeFlags_DefaultOpen }) {
                            callblInAssetOverridesFolder();
                        }
                    }
                }
            }
            };
        ImVec4 color_ModFolder(0.979f, 0.523f, 0.395f, 1.000f);
        ImVec4 color_Datapack(0.000f, 0.764f, 1.000f, 1.000f);
        ImVec4 color_LooseFile(0.425f, 0.780f, 0.392f, 1.000f);
        auto ExampleModInstall_VictoryOutfit = [&]() {
            ImGuiCTX::PushStyleColor _ct(ImGuiCol_Text, color_ModFolder);
            if (ImGuiCTX::TreeNodeEx _modFld{ "VictoryOutfit/   <<<--- All the .data files for your mod go here", ImGuiTreeNodeFlags_DefaultOpen }) {
                ImGuiCTX::PushStyleColor _ct(ImGuiCol_Text, color_Datapack);
                ImGui::BulletText("1_-_CN_P_FR_LegacyAvatar_Altair.data");
            }
            };
        auto ExampleModInstall_SwordOfAltair = [&]() {
            ImGuiCTX::PushStyleColor _ct(ImGuiCol_Text, color_ModFolder);
            if (ImGuiCTX::TreeNodeEx _{ "Sword of Altair/", ImGuiTreeNodeFlags_DefaultOpen }) {
                {
                    ImGuiCTX::PushStyleColor _ct(ImGuiCol_Text, color_LooseFile);
                    ImGuiCTX::TreeNodeEx _lfFolder{ "LooseFiles/   <<<--- All the \"loose files\" (non .data files) go here", ImGuiTreeNodeFlags_DefaultOpen };
                    if (_lfFolder) {
                        ImGui::BulletText("1_-_SwordOfEden_LOD0.mesh");
                    }
                }
                ImGuiCTX::PushStyleColor _ct(ImGuiCol_Text, color_Datapack);
                ImGui::BulletText("0_-_GFX_SwordOfEden_Glow_DiffuseMap.data");
                ImGui::BulletText("1_-_CN_W_SwordOfEden_DiffuseMap.data");
                ImGui::BulletText("1_-_CN_W_SwordOfEden_NormalMap.data");
                ImGui::BulletText("1_-_CN_W_SwordOfEden_SpecularMap.data");
            }
            };
        auto ExampleModInstall_AlternateWalk = [&]() {
            ImGuiCTX::PushStyleColor _ct(ImGuiCol_Text, color_ModFolder);
            if (ImGuiCTX::TreeNodeEx _{ "Alternate Walking Animation/", ImGuiTreeNodeFlags_DefaultOpen }) {
                {
                    ImGuiCTX::PushStyleColor _ct(ImGuiCol_Text, color_LooseFile);
                    ImGuiCTX::TreeNodeEx _lfFolder{ "LooseFiles/   <<<--- All the \"loose files\" (non .data files) go here", ImGuiTreeNodeFlags_DefaultOpen };
                    if (_lfFolder) {
                        ImGui::BulletText("1_-_xx_Nav_Walk_3-Loop.Animation");
                        ImGui::BulletText("1_-_xx_Nav_Walk_ShoulderTurnLt.Animation");
                        ImGui::BulletText("1_-_xx_Nav_Walk_ShoulderTurnRt.Animation");
                    }
                }
            }
            };
        if (ImGuiCTX::TreeNode _exampleUsage{ "Example: How to install a new outfit replacer" })
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
                "I'll use the \"Assassins Creed Victory Outfit\" v1.0 for AC Unity by LordOfThe9 as an example.\n"
            );
            ImGui::CopyToClipboardOnClick("https://www.nexusmods.com/assassinscreedunity/mods/136", "Click to copy link to clipboard");
            ImGui::Text(
                "Typical usage: installing a new outfit replacer.\n"
                "1. Download the mod.\n"
                "2. From the downloaded .zip file, extract the\n"
                "       \"1_-_CN_P_FR_LegacyAvatar_Altair.data\"\n"
                "       (you can rename the file, but the datapack needs to have extension \".data\")\n"
                "   into\n"
                "       \"Assassin's Creed Unity/ACUFixes/plugins/AssetOverrides/VictoryOutfit/\":\n"
            );
            ExampleModInstall(ExampleModInstall_VictoryOutfit);
            ImGui::Text(
                "   If your mod has multiple .data files\n"
                "   (for example the \"Templar Extremist Retexture + Remove Distance Glow\" by MaceoniK\n"
                "   has 30 .data files), put them all in the same folder.\n"
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
        if (ImGuiCTX::TreeNode _exampleUsage{ "Example: How to install a new weapon replacer (\"loose files\")" })
        {
            ImGui::Text(
                "Weapon meshes are all contained in the same datapack,\n"
                "so for compatibility with other mods the replacers usually\n"
                "provide the Meshes as separate, \"loose\" files.\n"
                "All the \"Loose Files\" for a mod need to be put into a separate folder."
            );
            ImGui::Text(
                "I'll use the \"Sword of Altair\" v0.0.2 by Halzoid98 as an example.\n"
            );
            ImGui::CopyToClipboardOnClick("https://www.nexusmods.com/assassinscreedunity/mods/123", "Click to copy link to clipboard");
            ImGui::Text(
                "1. Download the mod and unzip the files.\n"
                "2. The mod contains 5 files: 4 datapacks and 1 \"loose\" Mesh.\n"
                "   Arrange them like so (you can rename the files if you want to):"
            );
            ExampleModInstall(ExampleModInstall_SwordOfAltair);
            ImGui::Text(
                "3. Enable the mod in the Load Order, same as with the outfit example.\n"
                "   You will probably have to restart the game\n"
                "   if you want to turn _weapon_ mods on and off.\n"
                "   They are loaded very early after the game starts and don't get unloaded at all.\n"
                "   See the \"Limitations\" section."
            );
        }
        if (ImGuiCTX::TreeNode _exampleUsage{ "Example: How to install a few animation replacers (\"loose files\")" })
        {
            ImGui::Text(
                "There are no special steps to take here compared to other mods.\n"
                "Just read the previous two examples.\n"
                "Mod files are either the \".data\" or the \"loose files\".\n"
            );
            ImGui::Text(
                "I'll use the \"ACU-Alternate Walking animation\" V1 by Petrichor23\n"
            );
            ImGui::CopyToClipboardOnClick("https://www.nexusmods.com/assassinscreedunity/mods/280", "Click to copy link to clipboard");
            ImGui::Text(
                "just as another example: a mod that only has loose files.\n"
            );
            ImGui::Text(
                "1. Download the mod and arrange the files in the following way:\n"
            );
            ExampleModInstall(ExampleModInstall_AlternateWalk);
            ImGui::Text(
                "2. Enable the mod in the Load Order, same as with the outfit example.\n"
                "   You will probably have to restart the game\n"
                "   if you want to turn _animation_ mods on and off.\n"
                "   They are loaded very early after the game starts and don't get unloaded at all.\n"
                "   See the \"Limitations\" section."
            );
        }
        if (ImGuiCTX::TreeNode _limitations{ "Limitations:" })
        {
            ImGui::BulletText(
                "This is not a replacement for AnvilToolkit, merely a convenience feature\n"
                "to avoid unpacking/repacking large .forge files in _some_ cases.\n"
                "It is inconclusive for now whether or not the very act of swapping assets\n"
                "at runtime leads to _more_ crashes, but it doesn't seem to be the case.\n"
                "If you're experiencing crashes, you can install your mods\n"
                "the usual way via the AnvilToolkit\n"
                "which will always give you the most stable results.\n"
                "If you're suspecting that some crashes are caused by this feature,\n"
                "then just delete the\n"
                "    \"Assassin's Creed Unity/ACUFixes/plugins/AssetOverrides.dll\"\n"
                "to completely disable all Asset Overrides."
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
                "Sometimes you'll need to restart the game so that a mod can be freshly loaded,\n"
                "This is the case for all _weapon_ replacers, for example."
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
            ImGui::BulletText(
                "The game does not check for file errors, so don't put junk files\n"
                "into your mod folders.\n"
                "In particular, the game can freeze or crash even at startup\n"
                "if a .data file is not a correct datapack."
            );
        }
        if (ImGuiCTX::TreeNode _knownMods{ "Supported mods" })
        {
            ImGui::Text("Examples of mods I've confirmed to work when installed through Asset Overrides\n"
                "(click to copy to clipboard):");
            auto ModTextAndLink = [](const char* modName, const char* link)
                {
                    ImGui::BulletText(modName);
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip(link);
                    }
                    if (ImGui::IsItemClicked())
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
            ModTextAndLink("Connor's Tomahawk by Halzoid98", "https://www.nexusmods.com/assassinscreedunity/mods/114");
            ModTextAndLink("AC Valhalla Basim Sword by IBSARP", "https://www.nexusmods.com/assassinscreedunity/mods/147");
            ModTextAndLink("Excalibur Sword by Johntaber", "https://www.nexusmods.com/assassinscreedunity/mods/278");
            ModTextAndLink("Sword of Altair by Halzoid98", "https://www.nexusmods.com/assassinscreedunity/mods/123");
            ModTextAndLink("Classic Style Sync (Health) Regeneration by Petrichor23", "https://www.nexusmods.com/assassinscreedunity/mods/272");
            ModTextAndLink("Less Frequent World Events (Paris) by knEke", "https://www.nexusmods.com/assassinscreedunity/mods/299");
            ModTextAndLink("ACU-Alternate Walking animation by Petrichor23", "https://www.nexusmods.com/assassinscreedunity/mods/280");

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

            //ImGui::PushStyleColor(ImGuiCol_Text, colorIncompatible);
            //ImGui::PopStyleColor();
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
    AssetOverrides_InitFromLoadOrder();
}

#include "ReadFile.h"
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
std::vector<byte> GetSingleObjectOverrideBytes(uint64 handle)
{
    auto lookupResult = g_AssetModlist.FindSingleObjectOverrideForHandle(handle);
    if (!lookupResult.IsSuccessfullyRead()) return {};
    LOG_DEBUG(VirtualForgesLog,
        "[+] Loading Single Object: %s\n", lookupResult.m_AbsolutePath.u8string().c_str());
    return std::move(lookupResult.m_FileAsBytes);
}









void WhenNewForgeEntryWasJustAdded_ApplyCustomSorting(AllRegisters* params)
{
    AssetOverrides_PutForgesInCorrectOrder();
}
void WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement(uint64 rax, uint64 rbx, uint64 rsi, uint64 rbp, uint64 rsp, uint64 r13);
void WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement_ccimp(AllRegisters* params)
{
    WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement(params->GetRAX(), params->rbx_, params->rsi_, params->rbp_, params->GetRSP(), params->r13_);
}


AddVirtualForges::AddVirtualForges()
{
    uintptr_t whenGameForgesJustOpened = 0x14272F794;
    PresetScript_CCodeInTheMiddle(whenGameForgesJustOpened, 7,
        WhenNewForgeEntryWasJustAdded_ApplyCustomSorting, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    uintptr_t whenStartDeserializeFileInDatapackTogetherWithPrologue = 0x1426F0CF7;
    //PresetScript_CCodeInTheMiddle(whenStartDeserializeFileInDatapackTogetherWithPrologue, 5,
    //    WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement_ccimp, 0x1426F0DC7, false);

    // Normally the PresetScript_CCodeInTheMiddle() is good enough, but being a "JIT" code
    // it doesn't have the "unwind information", and so it breaks the stackwalk if any errors
    // occur during deserialization.
    // This means the error won't be caught by the CrashLogUnhandledExceptionHandler().
    // It can still be caught by the ZwRaiseException hook, but that one isn't enabled by default.
    // Same as Swapchain::Present(), the Deserialization is a "sensitive spot"
    // in terms of potential crashes because the game doesn't check for errors.
    // I want to give the CrashLog the best chance, so I'm using a customized bit of shellcode
    // to get only the registers I need and to preserve the intended return address stored in RSP.
    {
        DEFINE_ADDR(startDeserialize, whenStartDeserializeFileInDatapackTogetherWithPrologue);
        ALLOC(startDeserialize_cave, 0x80, whenStartDeserializeFileInDatapackTogetherWithPrologue);
        LABEL(label_startDeserialize_returnAddr);
        startDeserialize = {
            db(0xE8), RIP(startDeserialize_cave)
        };
        startDeserialize_cave = {
            "48 8b 0d", RIP(label_startDeserialize_returnAddr), //  - mov    rcx,QWORD PTR [label_startDeserialize_returnAddr]
            "48 89 0c 24"                                       //  - mov    QWORD PTR [rsp],rcx
            "48 89 c1"                                          //  - mov    rcx,rax
            "48 89 da"                                          //  - mov    rdx,rbx
            "49 89 f0"                                          //  - mov    r8,rsi
            "49 89 e9"                                          //  - mov    r9,rbp
            "48 89 64 24 28"                                    //  - mov    QWORD PTR [rsp+0x28],rsp
            "48 83 44 24 28 08"                                 //  - add    QWORD PTR [rsp+0x28],0x8
            "4c 89 6c 24 30"                                    //  - mov    QWORD PTR [rsp+0x30],r13
            "ff 25 00 00 00 00"                                 //  - jmp    QWORD PTR [rip+0]
            , dq((uintptr_t)&WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement)
            //label_startDeserialize_returnAddr:
                , PutLabel(label_startDeserialize_returnAddr)
                , dq((uintptr_t)0x1426F0DC7)
        };
    }
}
void AddVirtualForges::OnBeforeActivate()
{
    AssetOverrides_InitFromLoadOrder();
}
void AddVirtualForges::OnBeforeDeactivate()
{
    g_AssetModlist.Clear();
}

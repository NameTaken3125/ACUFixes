#include "pch.h"

#include "ACU/ForgeManager.h"
#include "Experimental_StrongPtr.h"
#include "AssetOverrides.h"
#include "Common_Plugins/ACUAllocs.h"
#include "Common_Plugins/Common_PluginSide.h"
#include "Serialization/Serialization.h"
#include "Serialization/NumericAdapters.h"
#include "Serialization/BooleanAdapter.h"
#include "Serialization/FilepathAdapter.h"
#include "Serialization/ToFromFile/ToFromFile.h"

#include "ImGuiCTX.h"

#include "MyLog.h"
static DEFINE_LOGGER_CONSOLE_AND_FILE(VirtualForgesLog, "[AssetOverrides]");
std::wstring asciiString2WString(const std::string& str);



DEFINE_GAME_FUNCTION(Allocate_mb, 0x1426F3760, void*, __fastcall, (__int64 p_numBytes, uint32 p_alignment_mb, void* currentHeap_mb));
DEFINE_GAME_FUNCTION(Allocate2, 0x1426D9A10, void*, __fastcall, (__int64 p_numBytes, unsigned int a2, void* resultOf0x1426F3760));
DEFINE_GAME_FUNCTION(BigArray__usedInEquipmentUnlocker, 0x142726010, void*, __fastcall, (void* bigArray, void* p_newBuffer, int p_prevSize, int a4, int a5, char a6, char a7, char a8, unsigned __int8 a9));

DEFINE_GAME_FUNCTION(stringHashCRC32_Cstring, 0x142759CF0, uint32, __fastcall, (char*));


template<typename T>
T* ACUAllocateDefaultConstruct()
{
    void* placementAddr = ACUAllocateBytes(sizeof(T), alignof(T));
    return new(placementAddr)T;
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
constexpr uint64 handle_EntityBuilder_Altair = 150860920202; // CN_P_FR_LegacyAvatar_Altair\CN_P_FR_LegacyAvatar_Altair.EntityBuilder
constexpr uint64 handle_EntityBuilder_SDN_Arno_BodyCust = 165671591321;
constexpr uint64 handle_EntityBuilder_Ezio = 151531263276; // CN_P_FR_LegacyAvatar_Ezio\CN_P_FR_LegacyAvatar_Ezio.EntityBuilder
constexpr uint64 handle_EntityBuilder_Connor = 150824510900; // CN_P_FR_LegacyAvatar_Connor\CN_P_FR_LegacyAvatar_Connor.EntityBuilder
constexpr uint64 handle_EntityBuilder_Bellec = 151252548870; // CN_P_LegacyAvatar_Bellec\CN_P_LegacyAvatar_Bellec.EntityBuilder
constexpr uint64 handle_EntityBuilder_Shay = 151641747367; // CN_A_FR_LegacyAvatar_Comet\CN_A_FR_LegacyAvatar_Comet.EntityBuilder
constexpr uint64 handle_EntityBuilder_McFarlane = 153024721147; // CN_P_LegacyAvatar_McFarlane\CN_P_LegacyAvatar_McFarlane.EntityBuilder

DEFINE_GAME_FUNCTION(ForgeManager__DecrementForgeEntryRefcount_mb, 0x142721FE0, void, __fastcall, (ForgeManager* a1, int p_forgeIdx));

ForgeFile_38* AllocateVirtualForgeContents(ForgeFile& newForge)
{
    ForgeFile_38* newDatapacksDesc = ACUAllocateDefaultConstruct<ForgeFile_38>();
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

    newForge.dirpathIfNotRootFolder_mb = ACUAllocateString(abspath.parent_path().string() + "\\");
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
    ForgeFile* newForge = ACUAllocateDefaultConstruct<ForgeFile>();
    newForge->someLock++;
    SetForgefilePathStrings(*newForge, absoluteFilepath);
    ForgeFile_DatapackPrefetchInfo* singleDatapackPrefetchInfo = BigArrayAppend(newForge->datapacksPrefetchInfoAscendingHandles);
    singleDatapackPrefetchInfo->firstHandle = targetHandle;
    singleDatapackPrefetchInfo->offsetInPrefetchData = 0;
    singleDatapackPrefetchInfo->prefetchDataSize = 4;
    singleDatapackPrefetchInfo->isCompressed_mb = false;
    newForge->prefetchData = ACUAllocateBytes(4, 0x10);
    std::memset(newForge->prefetchData, 0, 4);

    ForgeFile_38* newDatapacksDesc = AllocateVirtualForgeContents(*newForge);
    newForge->p38 = newDatapacksDesc;

    DatapackDescriptor* singleDatapackDesc = BigArrayAppend(newDatapacksDesc->arrDatapackDescs);
    singleDatapackDesc->handleFirstInDatapack = targetHandle;
    singleDatapackDesc->offsetInForgeFile = 0;
    singleDatapackDesc->packedSize = datapackSize;
    newDatapacksDesc->hashmap_datapackIdxByFirstHandle.Set(singleDatapackDesc->handleFirstInDatapack, 0);

    ForgeFileEntry* newForgeEntry = ACUAllocateDefaultConstruct<ForgeFileEntry>();
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
        SmallArrayAppend(fm->forges, newForgeEntry);
        break;
    case LoadPriority::Highest:
    default:
        SmallArrayInsert(fm->forges, newForgeEntry, 0);
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
            , L"[error] Failed to create virtual forge: \"%s\""
            , absoluteFilepath.wstring().c_str()
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
struct ResultForDetectedDatapack_LooksInvalid {};
struct ResultForDetectedDatapack_LookedOkButFailedToCreateVirtualForge {};
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
std::vector<std::unique_ptr<DetectedDatapack>> GetDatapacksFromFolder(const fs::path& modFolder)
{
    std::string extension = ".data";
    std::vector<std::unique_ptr<DetectedDatapack>> result;
    try {
        for (const auto& entry : fs::directory_iterator(modFolder)) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
                LOG_DEBUG(
                    VirtualForgesLog
                    , "  File  : %s\n"
                    , entry.path().filename().u8string().c_str()
                );
                std::optional<uint64> correspondingHandle = FindFirstHandleInDatapack(entry);
                if (correspondingHandle)
                {
                    LOG_DEBUG(
                        VirtualForgesLog
                        , "  Handle: %llu\n"
                        , *correspondingHandle
                    );
                    result.emplace_back(std::make_unique<DetectedDatapack>(
                        entry,
                        ResultForDetectedDatapack_LooksOk{ *correspondingHandle }
                    ));
                }
                else
                {
                    LOG_DEBUG(
                        VirtualForgesLog
                        , "[error]Not loading the folder \"%s\" because couldn't retrieve handle from \"%s\"\n"
                        , modFolder.filename().u8string().c_str()
                        , entry.path().filename().u8string().c_str()
                    );
                    result.emplace_back(std::make_unique<DetectedDatapack>(
                        entry,
                        ResultForDetectedDatapack_LooksInvalid{}
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
    std::vector<std::unique_ptr<DetectedDatapack>> m_Datapacks;
    bool m_IsActiveInLoadOrder = false;
    bool m_IsError = false;
    bool m_WasLoadedFromLoadOrderButNotFoundAnymore = false;

    AssetMod(const fs::path& modFolder_)
        : m_ModFolder(modFolder_)
    {}
    void GatherDatapacksList()
    {
        m_Datapacks = GetDatapacksFromFolder(m_ModFolder);
        m_IsError = std::find_if(m_Datapacks.begin(), m_Datapacks.end(), [](std::unique_ptr<DetectedDatapack>& datapack)
            {
                return std::get_if<ResultForDetectedDatapack_LooksOk>(&datapack->m_Result) == nullptr;
            }) != m_Datapacks.end();
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
class AssetModlist
{
public:
    fs::path m_AssetModsFolder;
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
    ImGuiTextBuffer FindAndFormatOriginalPrefetchingInfoForHandle(uint64 handle)
    {
        ImGuiTextBuffer buf;
        auto [nonmoddedForge, prefetchInfoIdx] = FindOriginalPrefetchingInfoForHandle(handle);
        if (!nonmoddedForge) return buf;
        ForgeFile_DatapackPrefetchInfo& prefetchInfo = nonmoddedForge->forgeContentsDescriptor->datapacksPrefetchInfoAscendingHandles[prefetchInfoIdx];
        buf.appendf(
            "Found prefetching info at idx %hd (size=%u, isCompressed=%s) in forge #%d (%s)"
            , prefetchInfoIdx
            , prefetchInfo.prefetchDataSize
            , prefetchInfo.isCompressed_mb ? "true" : "false"
            , nonmoddedForge->forgeIdx_mb
            , nonmoddedForge->forgeName
        );
        return buf;
    }

    void InitWithoutLoadOrder_old()
    {
        m_AssetModsFolder = AbsolutePathInThisDLLDirectory("AssetOverrides");
        m_Mods.clear();

        try {
            for (const auto& entry : fs::directory_iterator(m_AssetModsFolder)) {
                if (!entry.is_directory()) continue;
                LOG_DEBUG(VirtualForgesLog
                    , "Found mod folder: %s\n"
                    , entry.path().filename().u8string().c_str()
                );
                auto& newMod = m_Mods.emplace_back(std::make_unique<AssetMod>(entry.path()));
                newMod->GatherDatapacksList();
            }
        }
        catch (const fs::filesystem_error& e) {
            LOG_DEBUG(
                VirtualForgesLog
                , "[AddVirtualForges] Filesystem error: %s\n"
                , e.what()
            );
        }
    }
    void DrawMenu()
    {
        if (ImGui::Button("Save load order"))
        {
            LoadOrder_Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh load order"))
        {
            InitFromLoadOrder();
        }
        ImGui::Separator();
        ImGui::Text("Load Order (drag to reorder, right click on a mod to toggle)");
        ImGui::Separator();
        static ImGuiTextBuffer buf;
        const float leftPaneHeight = ImGui::GetTextLineHeightWithSpacing() * 20;
        const float leftPaneWidth = -ImGui::GetWindowContentRegionWidth() * 0.6f;
        static AssetMod* selectedMod = nullptr;
        if (ImGuiCTX::WindowChild _sectionMods{ "_sectionMods", ImVec2(leftPaneWidth, leftPaneHeight), true })
        {
            std::optional<size_t> idxDragged = {};
            std::optional<size_t> idxHovered = {};
            for (size_t n = 0; n < m_Mods.size(); n++)
            {
                auto& mod = m_Mods[n];
                std::u8string modName = mod->m_ModFolder.filename().u8string();
                buf.clear();
                const bool isActivated = mod->m_IsActiveInLoadOrder;
                buf.appendf(
                    "%2d. %s %s %s###%s"
                    , n
                    , isActivated ? "[+]" : "   "
                    , modName.c_str()
                    , mod->m_WasLoadedFromLoadOrderButNotFoundAnymore ? "(MISSING)" : ""
                    , modName.c_str()
                );
                const bool isSelected = mod.get() == selectedMod;
                const bool isModInvalid = mod->m_IsError;
                //ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
                std::optional<ImVec4> textColor = {};
                if (isModInvalid) textColor = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                else if (mod->m_Datapacks.size() == 0) textColor = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
                if (textColor) ImGui::PushStyleColor(ImGuiCol_Text, *textColor);
                const bool isClicked = ImGui::Selectable(buf.c_str(), isSelected);
                if (textColor) ImGui::PopStyleColor();
                const bool isActive = ImGui::IsItemActive();
                const bool isHoveredRect = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly);
                const bool isRightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
                //const bool isRightClicked = ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right);
                if (isActive)       idxDragged = n;
                if (isHoveredRect)  idxHovered = n;
                if (isClicked)      selectedMod = mod.get();
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
                        "%s: %d datapacks."
                        , mod->m_ModFolder.filename().u8string().c_str()
                        , mod->m_Datapacks.size()
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
                                ImGui::TableSetColumnIndex(0);
                                if (isDatapackInvalid) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
                                ImGui::Text((const char*)datapack->m_AbsolutePath.filename().u8string().c_str());
                                if (isDatapackInvalid) ImGui::PopStyleColor();

                                if (ImGui::IsItemHovered())
                                {
                                    if (datapackOk)
                                    {
                                        ImGuiTextBuffer buf = FindAndFormatOriginalPrefetchingInfoForHandle(datapackOk->m_Handle);
                                        ImGui::SetTooltip(buf.c_str());
                                    }
                                    else if (datapackInvalid)
                                    {
                                        ImGui::SetTooltip(
                                            "Does not look like a valid datapack.\n"
                                            "The game does not check for errors and can freeze, so I'm not loading it."
                                        );
                                    }
                                    else if (datapackFailedToLoad)
                                    {
                                        ImGui::SetTooltip(
                                            "Datapack looked OK when it was found, but the game failed to load it.\n"
                                            "Was the file removed? Try refreshing the Load Order."
                                        );
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
                                    ACU::WeakPtr<ManagedObject> weakRef(datapackOk->m_Handle);
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
            fs::path relPath = fs::relative(mod->m_ModFolder, m_AssetModsFolder);
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
    void InitFromLoadOrder()
    {
        m_AssetModsFolder = AbsolutePathInThisDLLDirectory("AssetOverrides");
        m_Mods.clear();

        const bool assetModsDirExists = fs::is_directory(m_AssetModsFolder);
        if (!assetModsDirExists)
        {
            LOG_DEBUG(VirtualForgesLog,
                "Asset mods folder not found:\n%s"
                , m_AssetModsFolder.u8string().c_str()
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
                auto& newMod = m_Mods.emplace_back(std::make_unique<AssetMod>(m_AssetModsFolder / Name));
                newMod->m_IsActiveInLoadOrder = Enable;
                newMod->m_WasLoadedFromLoadOrderButNotFoundAnymore = true;
            }
        }
        try {
            for (const auto& entry : fs::directory_iterator(m_AssetModsFolder)) {
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
                mod->GatherDatapacksList();
                mod->m_WasLoadedFromLoadOrderButNotFoundAnymore = false;
            }
        }
        catch (const fs::filesystem_error& e) {
            LOG_DEBUG(
                VirtualForgesLog
                , L"[AddVirtualForges] Filesystem error: %s\n"
                , utf8_and_wide_string_conversion::utf8_decode(e.what()).c_str()
            );
        }
        MakeVirtualForgesForAllDatapacks();
        ApplyCustomForgeOrder();
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
                    datapack->m_Result = ResultForDetectedDatapack_LookedOkButFailedToCreateVirtualForge();
                    mod->m_IsError = true;
                    break;
                }
            }
        }
    }
    void ToggleMod(AssetMod& mod, bool doActivate)
    {
        const bool wasActive = !doActivate;
        mod.m_IsActiveInLoadOrder = !mod.m_IsActiveInLoadOrder;
        auto* fm = ForgeManager::GetSingleton();
        if (!fm) return;
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
            SmallArrayInsert(fm.forges, forgeEntry, 0);
        }
    }
};
AssetModlist g_AssetModlist;


void WhenNewForgeEntryWasJustAdded_ApplyCustomSorting(AllRegisters* params)
{
    auto* fm = ForgeManager::GetSingleton();
    g_AssetModlist.MakeVirtualForgesForAllDatapacks();
    g_AssetModlist.ApplyCustomForgeOrder(*fm);
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
    ForgeFile_DatapackPrefetchInfo& originalPrefetchInfo = nonmoddedForge->forgeContentsDescriptor->datapacksPrefetchInfoAscendingHandles[prefetchInfoIdx];
    if (forgeContents->prefetchData)
    {
        ACUDeallocateBytes((byte*)forgeContents->prefetchData);
        byte* duplicatedOriginalPrefetchData = ACUAllocateBytes(originalPrefetchInfo.prefetchDataSize, 16);
        std::memcpy(
            duplicatedOriginalPrefetchData,
            &nonmoddedForge->forgeContentsDescriptor->prefetchData[originalPrefetchInfo.offsetInPrefetchData],
            originalPrefetchInfo.prefetchDataSize
        );
        forgeContents->prefetchData = duplicatedOriginalPrefetchData;
        // I'm assuming that in my "virtual forges" there is only a single datapack.
        ForgeFile_DatapackPrefetchInfo& singularDatapackPrefetchInfo = forgeContents->datapacksPrefetchInfoAscendingHandles[0];
        singularDatapackPrefetchInfo.isCompressed_mb = originalPrefetchInfo.isCompressed_mb;
        singularDatapackPrefetchInfo.prefetchDataSize = originalPrefetchInfo.prefetchDataSize;
        singularDatapackPrefetchInfo.offsetInPrefetchData = 0;
    }
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
                "       (if your mod has multiple .data files, put them all in the same folder)\n"
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
                "(and again, some datapacks are loaded too early for me to override - see\n"
                "   \"Game Bootstrap Settings.data\""
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

            ImGui::PushStyleColor(ImGuiCol_Text, colorWarning);
            ModTextAndLink(
                "Jacob's Outfits by th3kill (works OK, but seems to be prone to arbitrary crashes\n"
                "when used with other mods? Not sure why. You can still try it out)"
                , "https://www.nexusmods.com/assassinscreedunity/mods/217"
            );
            ModTextAndLink("Eiffel Tower by LoremIpsum12 (known to crash during fast travel, but works otherwise)", "https://www.nexusmods.com/assassinscreedunity/mods/225");
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Text, colorIncompatible);
            ImGui::Text("Mods that I cannot get to load using Asset Overrides:");
            ModTextAndLink(
                "Parkour Front flip and Jump mod by Petrichor23, because it uses\n"
                "    DataAI_GroupBehavior_Couple_TakeAway.data\n"
                "which is loaded too early for me to hook.)", "https://www.nexusmods.com/assassinscreedunity/mods/222");
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
























AddVirtualForges::AddVirtualForges()
{
    uintptr_t whenNewForgeEntryWasJustAdded = 0x142737D1D;
    PresetScript_CCodeInTheMiddle(whenNewForgeEntryWasJustAdded, 7,
        WhenNewForgeEntryWasJustAdded_ApplyCustomSorting, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    uintptr_t whenGatheringPrefetchInfoForDatapack = 0x1427338EA;
    PresetScript_CCodeInTheMiddle(whenGatheringPrefetchInfoForDatapack, 7,
        WhenGatheringPrefetchInfoForDatapack_FindOriginalPrefetchInfo, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}
void AddVirtualForges::OnBeforeActivate()
{
    g_AssetModlist.InitFromLoadOrder();
}
void AddVirtualForges::OnBeforeDeactivate()
{
    g_AssetModlist.Clear();
}

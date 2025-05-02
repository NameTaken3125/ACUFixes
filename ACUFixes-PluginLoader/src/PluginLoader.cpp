#include "pch.h"

#include "PluginLoader.h"
#include "MyLog.h"


MyPluginLoader g_MyPluginLoader;

bool IsPluginAPIversionCompatible(uint64 pluginLoaderVersion, uint64 pluginVersion)
{
    // If the pluginloader updates introduce changes that would break old plugins,
    // I can list the breaking versions right here.
    return pluginLoaderVersion >= pluginVersion;
}
#define PLUGIN_API_VERSION_GET_MAJOR(version) ((version >> 24) & 0xFF)
#define PLUGIN_API_VERSION_GET_MINOR(version) ((version >> 16) & 0xFF)
#define PLUGIN_API_VERSION_GET_MINORER(version) ((version >> 8) & 0xFF)
#define PLUGIN_API_VERSION_GET_MINOREST(version) (version & 0xFF)
void MyPluginLoader::LoadAndStartPlugin(MyPluginResult& pluginRecord)
{
    const auto* pluginFilepath = pluginRecord.m_filepath.c_str();
    fs::path _pluginName = pluginRecord.m_filepath.filename().c_str();
    const auto* pluginName = _pluginName.c_str();
    LOG_DEBUG(DefaultLogger, L"[*] Plugin %s: Trying to load.\n", pluginFilepath);
    HMODULE moduleHandle = LoadLibraryW(pluginFilepath);
    if (!moduleHandle)
    {
        LOG_DEBUG(DefaultLogger, L"[x] Plugin %s: Failed to load.\n", pluginName);
        return;
    }
    LOG_DEBUG(DefaultLogger, L"[*] Plugin %s: handle: %llX\n", pluginName, moduleHandle);
    ACUPluginStart_fnt startFn = reinterpret_cast<ACUPluginStart_fnt>(GetProcAddress(moduleHandle, "ACUPluginStart"));
    if (!startFn)
    {
        LOG_DEBUG(DefaultLogger, L"[x] Plugin %s: Doesn't export the `ACUPluginStart()` function. Unloading.\n", pluginName);
        FreeLibrary(moduleHandle);
        return;
    }
    LOG_DEBUG(DefaultLogger, L"[*] Plugin %s: Address of the `ACUPluginStart()` function: %llX\n", pluginName, startFn);
    std::unique_ptr<ACUPluginInfo> pluginInfo = std::make_unique<ACUPluginInfo>();
    bool isPluginThinksIsReadyToStart = startFn(m_PluginLoaderInterfaces, *pluginInfo);
    if (!isPluginThinksIsReadyToStart)
    {
        LOG_DEBUG(DefaultLogger, L"[x] Plugin %s: `ACUPluginStart()` returned `false`. Unloading.\n", pluginName);
        FreeLibrary(moduleHandle);
        return;
    }
    LOG_DEBUG(DefaultLogger, L"[*] Plugin %s: `ACUPluginStart()` returned `true`. Plugin API version: %d.%d.%d.%d. Plugin's version: 0x%llX.\n"
        , pluginName
        , PLUGIN_API_VERSION_GET_MAJOR(pluginInfo->m_PluginAPIVersion)
        , PLUGIN_API_VERSION_GET_MINOR(pluginInfo->m_PluginAPIVersion)
        , PLUGIN_API_VERSION_GET_MINORER(pluginInfo->m_PluginAPIVersion)
        , PLUGIN_API_VERSION_GET_MINOREST(pluginInfo->m_PluginAPIVersion)
        , pluginInfo->m_PluginVersion
    );
    if (!IsPluginAPIversionCompatible(g_CurrentPluginAPIversion, pluginInfo->m_PluginAPIVersion))
    {
        LOG_DEBUG(DefaultLogger, L"[x] Plugin %s: Plugin's API version %d.%d.%d.%d is incompatible with the version of this plugin loader (%d.%d.%d.%d). Unloading.\n"
            , pluginName
            , PLUGIN_API_VERSION_GET_MAJOR(pluginInfo->m_PluginAPIVersion)
            , PLUGIN_API_VERSION_GET_MINOR(pluginInfo->m_PluginAPIVersion)
            , PLUGIN_API_VERSION_GET_MINORER(pluginInfo->m_PluginAPIVersion)
            , PLUGIN_API_VERSION_GET_MINOREST(pluginInfo->m_PluginAPIVersion)
            , PLUGIN_API_VERSION_GET_MAJOR(g_CurrentPluginAPIversion)
            , PLUGIN_API_VERSION_GET_MINOR(g_CurrentPluginAPIversion)
            , PLUGIN_API_VERSION_GET_MINORER(g_CurrentPluginAPIversion)
            , PLUGIN_API_VERSION_GET_MINOREST(g_CurrentPluginAPIversion)
        );
        FreeLibrary(moduleHandle);
        return;
    }
    if (!pluginInfo->m_Start)
    {
        LOG_DEBUG(DefaultLogger, L"[x] Plugin %s: Plugin didn't provide the `ACUPluginInfo.m_Start()` callback. Unloading.\n", pluginName);
        FreeLibrary(moduleHandle);
        return;
    }
    if (!pluginInfo->m_Start(m_PluginLoaderInterfaces))
    {
        LOG_DEBUG(DefaultLogger, L"[x] Plugin %s: The start procedure returned `false` indicating that the plugin doesn't want to start. Unloading.\n", pluginName);
        FreeLibrary(moduleHandle);
        return;
    }
    LOG_DEBUG(DefaultLogger, L"[+] Plugin %s: Successfully loaded.\n", pluginName);
    pluginRecord.m_successfulLoad.emplace(moduleHandle, std::move(pluginInfo));
}
#undef PLUGIN_API_VERSION_GET_MAJOR
#undef PLUGIN_API_VERSION_GET_MINOR
#undef PLUGIN_API_VERSION_GET_MINORER
#undef PLUGIN_API_VERSION_GET_MINOREST
void MyPluginLoader::UnloadAllPlugins()
{
    for (std::unique_ptr<MyPluginResult>& dll : dllResults)
    {
        if (dll->m_successfulLoad)
        {
            FreeLibrary(dll->m_successfulLoad->m_moduleHandle);
            dll->m_successfulLoad.reset();
        }
    }
}
void MyPluginLoader::RequestUnloadDLL(HMODULE dllHandle)
{
    for (std::unique_ptr<MyPluginResult>& loadedPlugin : dllResults)
    {
        if (loadedPlugin->m_successfulLoad
            && loadedPlugin->m_successfulLoad->m_moduleHandle == dllHandle)
        {
            LOG_DEBUG(DefaultLogger, L"[*] Plugin %s: Requested to unload.\n", loadedPlugin->m_filepath.filename().c_str());
            loadedPlugin->m_successfulLoad->m_isRequestedToUnload = true;
            m_IsRequestedToUnloadPlugin = true;
            break;
        }
    }
}

HMODULE PluginLoader_GetPluginIfLoaded(const wchar_t* pluginName)
{
    return g_MyPluginLoader.GetPluginIfLoaded(pluginName);
}
void PluginLoader_RequestUnloadPlugin(HMODULE dllHandle)
{
    g_MyPluginLoader.RequestUnloadDLL(dllHandle);
}
HMODULE MyPluginLoader::GetPluginIfLoaded(const wchar_t* pluginName)
{
    for (auto& plugin : dllResults)
    {
        if (!plugin->m_successfulLoad) { continue; }
        if (plugin->m_filepath.filename() == pluginName)
        {
            return plugin->m_successfulLoad->m_moduleHandle;
        }
    }
    return nullptr;
}
fs::path AbsolutePathInMyDirectory(const fs::path& filenameRel);
void MyPluginLoader::UpdateListOfAvailablePlugins()
{
    LOG_DEBUG(DefaultLogger, L"[*] UpdateListOfAvailablePlugins():\n");
    fs::path pluginsFolder = AbsolutePathInMyDirectory(L"plugins");
    fs::create_directory(pluginsFolder);
    std::vector<fs::path> currentlyPresentDLLsInFolder;
    currentlyPresentDLLsInFolder.reserve(32);
    for (const auto& entry : std::filesystem::directory_iterator(pluginsFolder)) {
        if (entry.path().extension() == L".dll") {
            currentlyPresentDLLsInFolder.push_back(entry.path());
            LOG_DEBUG(DefaultLogger, L"[*] Found DLL: %s\n", entry.path().c_str());
        }
    }
    // Remove the records of DLLs that were available but are not anymore.
    auto IsPluginRecordPresentButDLLisNoLongerAvailable = [&](std::unique_ptr<MyPluginResult>& alreadyDetectedPlugin)
        {
            if (alreadyDetectedPlugin->m_successfulLoad)
            {
                // If the plugin was successfully loaded, don't remove it from the list even if the file
                // was somehow removed.
                return false;
            }
            for (fs::path& presentDLLpath : currentlyPresentDLLsInFolder)
            {
                if (presentDLLpath == alreadyDetectedPlugin->m_filepath)
                {
                    // Though this plugin isn't loaded, a record of it is already present.
                    return false;
                }
            }
            // Plugin isn't loaded and though a record of it was present, the DLL is no longer available in the "plugins" folder.
            // Remove the record of it.
            return true;
        };
    dllResults.erase(std::remove_if(dllResults.begin(), dllResults.end(),
        IsPluginRecordPresentButDLLisNoLongerAvailable), dllResults.end());
    // Add records for newly detected DLLs.
    for (fs::path& presentDLLpath : currentlyPresentDLLsInFolder) {
        bool isRecordAlreadyPresentAboutThisDLL = false;
        for (std::unique_ptr<MyPluginResult>& alreadyDetectedPlugin : dllResults)
        {
            if (alreadyDetectedPlugin->m_filepath == presentDLLpath)
            {
                isRecordAlreadyPresentAboutThisDLL = true;
            }
        }
        if (isRecordAlreadyPresentAboutThisDLL)
        {
            continue;
        }
        std::unique_ptr<MyPluginResult> result = std::make_unique<MyPluginResult>(presentDLLpath);
        dllResults.push_back(std::move(result));
    }
}
void MyPluginLoader::LoadAllFoundNonloadedPlugins()
{
    LOG_DEBUG(DefaultLogger, L"[*] LoadAllFoundNonloadedPlugins():\n");
    for (std::unique_ptr<MyPluginResult>& plugin : dllResults)
    {
        if (plugin->m_successfulLoad)
        {
            continue;
        }
        LoadAndStartPlugin(*plugin);
    }
}
#include "Common_Plugins_impl/PluginLoaderSharedGlobals.h"
extern ImGuiContext* GImGui;
void MyPluginLoader::DrawPluginListControls()
{
    ImGui::Text("Found plugins: %d", dllResults.size());
    if (ImGui::Button("Update available plugin list"))
    {
        UpdateListOfAvailablePlugins();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load all nonloaded"))
    {
        LoadAllFoundNonloadedPlugins();
    }
    std::string buf;

    ImGuiShared imguiShared{ *GImGui };
    ImGui::GetAllocatorFunctions(&imguiShared.alloc_func, &imguiShared.free_func, &imguiShared.user_data);
    for (size_t i = 0; i < dllResults.size(); i++)
    {
        std::unique_ptr<MyPluginResult>& plugin = dllResults[i];
        buf = std::to_string(i + 1) + ". " + plugin->m_filepath.filename().string();
        ImGui::PushID(&plugin);
        if (plugin->m_successfulLoad)
        {
            auto DrawSuccessfullyLoadedPlugin = [&]()
                {
                    auto* menuCallback = plugin->m_successfulLoad->m_pluginInterface->m_EveryFrameWhenMenuIsOpen;
                    if (!menuCallback)
                    {
                        ImGui::Text(buf.c_str());
                        return;
                    }
                    bool& isSeparateMenu = plugin->m_successfulLoad->m_isMenuOpen;
                    ImGuiTreeNodeFlags treeflags = ImGuiTreeNodeFlags_FramePadding;
                    bool isMightBeExpanded = true;
                    if (isSeparateMenu)
                    {
                        treeflags |= ImGuiTreeNodeFlags_Leaf;
                        isMightBeExpanded = false;
                    }
                    bool treeopen = ImGui::TreeNodeEx(
                        buf.c_str(),
                        treeflags
                    );
                    ImGui::SameLine();
                    if (ImGui::Button("Toggle menu"))
                    {
                        isSeparateMenu = !isSeparateMenu;
                    }
                    if (treeopen) {
                        if (isMightBeExpanded) {
                            ImGui::Separator();
                            menuCallback(imguiShared);
                            ImGui::Separator();
                        }
                        ImGui::TreePop();
                    }
                };
            DrawSuccessfullyLoadedPlugin();
        }
        else
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled(buf.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Load"))
            {
                LoadAndStartPlugin(*plugin);
            }
        }
        ImGui::PopID();
        buf.clear();
    }
}
void MyPluginLoader::DrawImGuiForPlugins_WhenMenuIsOpened()
{
    ImGuiShared imguiShared{ *GImGui };
    ImGui::GetAllocatorFunctions(&imguiShared.alloc_func, &imguiShared.free_func, &imguiShared.user_data);
    for (std::unique_ptr<MyPluginResult>& plugin : g_MyPluginLoader.dllResults)
    {
        if (!plugin->m_successfulLoad) { continue; }
        if (!plugin->m_successfulLoad->m_isMenuOpen) { continue; }
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
        if (auto* menuCallback = plugin->m_successfulLoad->m_pluginInterface->m_EveryFrameWhenMenuIsOpen) {
            if (ImGui::Begin(plugin->m_filepath.filename().string().c_str(), &plugin->m_successfulLoad->m_isMenuOpen)) {
                menuCallback(imguiShared);
            }
            ImGui::End();
        }
    }
}
void MyPluginLoader::DrawImGuiForPlugins_EvenWhenMenuIsClosed()
{
    ImGuiShared imguiShared{ *GImGui };
    ImGui::GetAllocatorFunctions(&imguiShared.alloc_func, &imguiShared.free_func, &imguiShared.user_data);
    for (const std::unique_ptr<MyPluginResult>& plugin : g_MyPluginLoader.dllResults)
    {
        if (!plugin->m_successfulLoad) { continue; }
        if (auto* everyFrameCallback = plugin->m_successfulLoad->m_pluginInterface->m_EveryFrameEvenWhenMenuIsClosed)
        {
            everyFrameCallback(imguiShared);
        }
    }
}

void MyPluginLoader::EveryFrameBeforeGraphicsUpdate()
{
    if (m_IsRequestedToUnloadPlugin)
    {
        for (std::unique_ptr<MyPluginResult>& loadedPlugin : dllResults)
        {
            if (loadedPlugin->m_successfulLoad
                && loadedPlugin->m_successfulLoad->m_isRequestedToUnload)
            {
                FreeLibrary(loadedPlugin->m_successfulLoad->m_moduleHandle);
                loadedPlugin->m_successfulLoad.reset();
            }
        }
        m_IsRequestedToUnloadPlugin = false;
    }
}

void EveryFrameBeforeGraphicsUpdate()
{
    g_MyPluginLoader.EveryFrameBeforeGraphicsUpdate();
}
void DrawPluginListControls()
{
    g_MyPluginLoader.DrawPluginListControls();
}
void DrawPluginsWhenMenuOpen()
{
    g_MyPluginLoader.DrawImGuiForPlugins_WhenMenuIsOpened();
}
void DrawPluginsEvenWhenMenuIsClosed()
{
    g_MyPluginLoader.DrawImGuiForPlugins_EvenWhenMenuIsClosed();
}
void PluginLoader_FindAndLoadPlugins()
{
    g_MyPluginLoader.UpdateListOfAvailablePlugins();
    g_MyPluginLoader.LoadAllFoundNonloadedPlugins();
}

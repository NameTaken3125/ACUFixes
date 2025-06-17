#include "pch.h"

#include "PluginLoader.h"
#include "MyLog.h"

bool IsPluginAPIversionCompatible(uint64 apiVersion_pluginLoader, uint64 apiVersion_plugin)
{
    // If the pluginloader updates introduce changes that would break old plugins,
    // I can list the breaking versions right here.
    if (apiVersion_pluginLoader >= MAKE_PLUGIN_LOADER_VERSION(0, 9, 0, 0) &&
        apiVersion_plugin       <  MAKE_PLUGIN_LOADER_VERSION(0, 9, 0, 0))
        return false;
    return apiVersion_pluginLoader >= apiVersion_plugin;
}
#define PLUGIN_API_VERSION_GET_MAJOR(version) ((version >> 24) & 0xFF)
#define PLUGIN_API_VERSION_GET_MINOR(version) ((version >> 16) & 0xFF)
#define PLUGIN_API_VERSION_GET_MINORER(version) ((version >> 8) & 0xFF)
#define PLUGIN_API_VERSION_GET_MINOREST(version) (version & 0xFF)
BOOL FreeDLL(HMODULE hLibModule)
{
    BOOL result = FreeLibrary(hLibModule);
    //CoFreeUnusedLibraries();
    return result;
}
void MyPluginLoader::LoadPluginCheckCompatibilityAndCompleteInitialization(MyPluginResult& pluginRecord)
{
    LoadPluginAndCheckCompatibility(pluginRecord);
    if (!pluginRecord.m_successfulLoad) return;
    auto& pluginInfo = pluginRecord.m_successfulLoad->m_pluginInterface;
    HMODULE moduleHandle = pluginRecord.m_successfulLoad->m_moduleHandle;
    const char* pluginName = pluginRecord.m_PluginName.c_str();
    if (pluginInfo->m_InitStage_WhenCodePatchesAreSafeToApply)
    {
        if (!pluginInfo->m_InitStage_WhenCodePatchesAreSafeToApply(m_PluginLoaderInterfaces))
        {
            LOG_DEBUG(DefaultLogger, "[error][x] Plugin %s: The \"m_InitStage_WhenCodePatchesAreSafeToApply()\" procedure returned `false` indicating that the plugin doesn't want to start. Unloading.\n", pluginName);
            pluginRecord.m_successfulLoad.reset();
            FreeDLL(moduleHandle);
            return;
        }
    }
}
void MyPluginLoader::LoadPluginAndCheckCompatibility(MyPluginResult& pluginRecord)
{
    std::wstring pluginFilepath = pluginRecord.m_filepath.wstring();
    const char* pluginName = pluginRecord.m_PluginName.c_str();
    LOG_DEBUG(DefaultLogger, "[*] Plugin %s: Trying to load.\n", utf8_and_wide_string_conversion::utf8_encode(pluginFilepath).c_str());
    HMODULE moduleHandle = nullptr;
    {
        // Below is a way to check the plugin DLLs for exported functions
        // without running the DllMain or any of global constructors.
        // Seems to work OK, I'm just not sure I need this.
        // I guess this can prevent some crashes when a completely arbitrary DLL
        // is thrown into the plugins/ folder by accident, then was loaded,
        // then was very quickly unloaded without warning.
        moduleHandle = LoadLibraryExW(pluginFilepath.c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES);
        if (!moduleHandle)
        {
            LOG_DEBUG(DefaultLogger, "[error][x] Plugin %s: Failed to load.\n", pluginName);
            return;
        }
        LOG_DEBUG(DefaultLogger, "[*] Plugin %s: Temporary handle: %llX\n", pluginName, moduleHandle);
        ACUPluginStart_fnt startFn = reinterpret_cast<ACUPluginStart_fnt>(GetProcAddress(moduleHandle, "ACUPluginStart"));
        if (!startFn)
        {
            LOG_DEBUG(DefaultLogger, "[error][x] Plugin %s: Doesn't export the `ACUPluginStart()` function. Unloading.\n", pluginName);
            FreeDLL(moduleHandle);
            return;
        }
        FreeDLL(moduleHandle);
        moduleHandle = nullptr;
    }
    moduleHandle = LoadLibraryW(pluginFilepath.c_str());
    if (!moduleHandle)
    {
        LOG_DEBUG(DefaultLogger, "[error][x] Plugin %s: Failed to load.\n", pluginName);
        return;
    }
    LOG_DEBUG(DefaultLogger, "[*] Plugin %s: handle: %llX\n", pluginName, moduleHandle);
    ACUPluginStart_fnt startFn = reinterpret_cast<ACUPluginStart_fnt>(GetProcAddress(moduleHandle, "ACUPluginStart"));
    if (!startFn)
    {
        LOG_DEBUG(DefaultLogger, "[error][x] Plugin %s: Doesn't export the `ACUPluginStart()` function. Unloading.\n", pluginName);
        FreeDLL(moduleHandle);
        return;
    }
    LOG_DEBUG(DefaultLogger, "[*] Plugin %s: Address of the `ACUPluginStart()` function: %llX\n", pluginName, startFn);
    std::unique_ptr<ACUPluginInfo> pluginInfo = std::make_unique<ACUPluginInfo>();
    bool isPluginThinksIsReadyToStart = startFn(m_PluginLoaderInterfaces, *pluginInfo);
    if (!isPluginThinksIsReadyToStart)
    {
        LOG_DEBUG(DefaultLogger, "[error][x] Plugin %s: `ACUPluginStart()` returned `false`. Unloading.\n", pluginName);
        FreeDLL(moduleHandle);
        return;
    }
    LOG_DEBUG(DefaultLogger, "[*] Plugin %s: `ACUPluginStart()` returned `true`. Plugin API version: %d.%d.%d.%d. Plugin's version: 0x%llX.\n"
        , pluginName
        , PLUGIN_API_VERSION_GET_MAJOR(pluginInfo->m_PluginAPIVersion)
        , PLUGIN_API_VERSION_GET_MINOR(pluginInfo->m_PluginAPIVersion)
        , PLUGIN_API_VERSION_GET_MINORER(pluginInfo->m_PluginAPIVersion)
        , PLUGIN_API_VERSION_GET_MINOREST(pluginInfo->m_PluginAPIVersion)
        , pluginInfo->m_PluginVersion
    );
    if (!IsPluginAPIversionCompatible(g_CurrentPluginAPIversion, pluginInfo->m_PluginAPIVersion))
    {
        LOG_DEBUG(DefaultLogger, "[error][x] Plugin %s: Plugin's API version %d.%d.%d.%d is incompatible with the version of this plugin loader (%d.%d.%d.%d). Unloading.\n"
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
        FreeDLL(moduleHandle);
        return;
    }
    if (pluginInfo->m_InitStage_WhenVersionsAreDeemedCompatible)
        pluginInfo->m_InitStage_WhenVersionsAreDeemedCompatible(m_PluginLoaderInterfaces);
    LOG_DEBUG(DefaultLogger, "[+] Plugin \"%s\": Initial loading and API compatibility check successful.\n", pluginName);
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
            FreeDLL(dll->m_successfulLoad->m_moduleHandle);
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
            LOG_DEBUG(DefaultLogger, "[*] Plugin %s: Requested to unload.\n", loadedPlugin->m_PluginName.c_str());
            loadedPlugin->m_successfulLoad->m_isRequestedToUnload = true;
            m_IsRequestedToUnloadPlugin = true;
            break;
        }
    }
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
    LOG_DEBUG(DefaultLogger, "[*] UpdateListOfAvailablePlugins():\n");
    fs::path pluginsFolder = AbsolutePathInMyDirectory(L"plugins");
    fs::create_directory(pluginsFolder);
    std::vector<fs::path> currentlyPresentDLLsInFolder;
    currentlyPresentDLLsInFolder.reserve(32);
    for (const auto& entry : std::filesystem::directory_iterator(pluginsFolder)) {
        if (entry.path().extension() == L".dll") {
            currentlyPresentDLLsInFolder.push_back(entry.path());
            LOG_DEBUG(DefaultLogger, "[*] Found DLL: %s\n", entry.path().u8string().c_str());
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
void MyPluginLoader::LoadAllFoundNonloadedPluginsCheckCompatibilityAndCompleteInitialization()
{
    LOG_DEBUG(DefaultLogger, "[*] LoadAllFoundNonloadedPluginsCheckCompatibilityAndCompleteInitialization():\n");
    for (std::unique_ptr<MyPluginResult>& plugin : dllResults)
    {
        if (plugin->m_successfulLoad)
        {
            continue;
        }
        LoadPluginCheckCompatibilityAndCompleteInitialization(*plugin);
    }
}
void MyPluginLoader::LoadAllFoundNonloadedPluginsAndCheckCompatibility()
{
    LOG_DEBUG(DefaultLogger, "[*] LoadAllFoundNonloadedPluginsAndCheckCompatibility():\n");
    for (std::unique_ptr<MyPluginResult>& plugin : dllResults)
    {
        if (plugin->m_successfulLoad)
        {
            continue;
        }
        LoadPluginAndCheckCompatibility(*plugin);
    }
}
void MyPluginLoader::FirstTimeGatherPluginsAndCheckCompatibility()
{
    UpdateListOfAvailablePlugins();
    LoadAllFoundNonloadedPluginsAndCheckCompatibility();
    LOG_DEBUG(DefaultLogger, "[+] FirstTimeGatherPluginsAndCheckCompatibility() finished.\n");
}
void MyPluginLoader::WhenGameCodeIsUnpacked()
{
    LOG_DEBUG(DefaultLogger,
        "[*] Early hook: When Game code is unpacked.\n"
    );
    for (auto& plugin : this->dllResults)
    {
        if (!plugin->m_successfulLoad) continue;
        auto* callback = plugin->m_successfulLoad->m_pluginInterface->m_EarlyHook_WhenGameCodeIsUnpacked;
        if (callback)
        {
            LOG_DEBUG(DefaultLogger,
                "[*] Plugin \"%s\": calling the \"m_EarlyHook_WhenGameCodeIsUnpacked()\" callback.\n"
                , plugin->m_PluginName.c_str()
            );
            callback();
        }
        else
        {
            LOG_DEBUG(DefaultLogger,
                "[*] Plugin \"%s\" does not provide a \"m_EarlyHook_WhenGameCodeIsUnpacked()\" callback.\n"
                , plugin->m_PluginName.c_str()
            );
        }
    }
}
void MyPluginLoader::WhenSafeToApplyCodePatches()
{
    LOG_DEBUG(DefaultLogger, "[*] WhenSafeToApplyCodePatches():\n");
    for (std::unique_ptr<MyPluginResult>& plugin : dllResults)
    {
        if (!plugin->m_successfulLoad)
        {
            continue;
        }
        const bool isWantsToContinue = plugin->m_successfulLoad->m_pluginInterface->m_InitStage_WhenCodePatchesAreSafeToApply(m_PluginLoaderInterfaces);
        if (!isWantsToContinue)
        {
            HMODULE moduleHandle = plugin->m_successfulLoad->m_moduleHandle;
            plugin->m_successfulLoad.reset();
            FreeDLL(moduleHandle);
        }
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
        LoadAllFoundNonloadedPluginsCheckCompatibilityAndCompleteInitialization();
    }
    ImGuiTextBuffer buf;

    ImGuiShared imguiShared{ *GImGui };
    ImGui::GetAllocatorFunctions(&imguiShared.alloc_func, &imguiShared.free_func, &imguiShared.user_data);
    for (size_t i = 0; i < dllResults.size(); i++)
    {
        std::unique_ptr<MyPluginResult>& plugin = dllResults[i];
        buf.appendf("%d. %s", i + 1, plugin->m_PluginName.c_str());
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
                            if (ImGui::BeginChild("embeddedMenu", ImVec2(-FLT_MIN, 0.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY))
                            {
                                menuCallback(imguiShared);
                            }
                            ImGui::EndChild();
                            ImGui::Separator();
                        }
                        ImGui::TreePop();
                    }
                };
            DrawSuccessfullyLoadedPlugin();
        }
        else
        {
            ImGui::BeginDisabled();
            ImGui::AlignTextToFramePadding();
            ImGui::Bullet();
            ImGui::Text(buf.c_str());
            ImGui::EndDisabled();
            ImGui::SameLine();
            if (ImGui::Button("Load"))
            {
                LoadPluginCheckCompatibilityAndCompleteInitialization(*plugin);
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
    for (std::unique_ptr<MyPluginResult>& plugin : dllResults)
    {
        if (!plugin->m_successfulLoad) { continue; }
        if (!plugin->m_successfulLoad->m_isMenuOpen) { continue; }
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
        if (auto* menuCallback = plugin->m_successfulLoad->m_pluginInterface->m_EveryFrameWhenMenuIsOpen) {
            if (ImGui::Begin(plugin->m_PluginName.c_str(), &plugin->m_successfulLoad->m_isMenuOpen, ImGuiWindowFlags_NoFocusOnAppearing)) {
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
    for (const std::unique_ptr<MyPluginResult>& plugin : dllResults)
    {
        if (!plugin->m_successfulLoad) { continue; }
        if (auto* everyFrameCallback = plugin->m_successfulLoad->m_pluginInterface->m_EveryFrameEvenWhenMenuIsClosed)
        {
            everyFrameCallback(imguiShared);
        }
    }
}

#include "ImGuiConsole.h"
#include "base.h"
static void DoDefaultInputCapture()
{
    const bool blockKeyboard = Base::Data::ShowMenu || g_ConsoleMode == ConsoleMode::ForegroundAndFocusable;
    const bool blockMouse = (Base::Data::ShowMenu || g_ConsoleMode == ConsoleMode::ForegroundAndFocusable) && !ImGui::IsKeyDown(ImGuiKey_ModAlt);
    ImGui::SetNextFrameWantCaptureMouse(blockMouse);
    ImGui::SetNextFrameWantCaptureKeyboard(blockKeyboard);
    ImGui::GetIO().MouseDrawCursor = Base::Data::ShowMenu || g_ConsoleMode == ConsoleMode::ForegroundAndFocusable;
}
void MyPluginLoader::EveryFrameBeforeGraphicsUpdate()
{
    DoDefaultInputCapture();

    if (m_IsRequestedToUnloadPlugin)
    {
        for (std::unique_ptr<MyPluginResult>& loadedPlugin : dllResults)
        {
            if (loadedPlugin->m_successfulLoad
                && loadedPlugin->m_successfulLoad->m_isRequestedToUnload)
            {
                FreeDLL(loadedPlugin->m_successfulLoad->m_moduleHandle);
                loadedPlugin->m_successfulLoad.reset();
            }
        }
        m_IsRequestedToUnloadPlugin = false;
    }
}


MyPluginLoader g_MyPluginLoader;


HMODULE PluginLoader_GetPluginIfLoaded(const wchar_t* pluginName)
{
    return g_MyPluginLoader.GetPluginIfLoaded(pluginName);
}
void PluginLoader_RequestUnloadPlugin(HMODULE dllHandle)
{
    g_MyPluginLoader.RequestUnloadDLL(dllHandle);
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
void PluginLoader_FirstTimeGatherPluginsAndCheckCompatibility()
{
    g_MyPluginLoader.FirstTimeGatherPluginsAndCheckCompatibility();
}
void PluginLoader_WhenGameCodeIsUnpacked()
{
    g_MyPluginLoader.WhenGameCodeIsUnpacked();
}
void PluginLoader_WhenSafeToApplyCodePatches()
{
    g_MyPluginLoader.WhenSafeToApplyCodePatches();
}

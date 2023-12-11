#include "pch.h"

#include "base.h"
#include "ImGuiCTX.h"

#include "PluginLoaderConfig.h"
#include "ImGuiConfigUtils.h"

void DrawSuccessfulInjectionIndicatorOverlay()
{
    ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    window_flags |= ImGuiWindowFlags_NoNav;
    window_flags |= ImGuiWindowFlags_NoInputs;

    if (ImGui::Begin("Always enabled overlay", nullptr, window_flags))
    {
        ImGui::Text(
            "Press %s to open ImGui menu."
            "\nThe hotkeys can be changed in the menu or in the config file."
            "\nThis text can be disabled in the Extra tab of the menu. See `ACUFixes-readme.txt`."
            , enum_reflection<VirtualKeys>::GetString(g_PluginLoaderConfig.hotkey_ToggleMenu)
            );
    }
    ImGui::End();
}
std::filesystem::path& GetThisDLLAbsolutePath();
void DrawModMenuControls()
{
    ImGui::Text(
        "Mod menu hotkeys."
        "\nYou can also change the hotkeys by manually editing"
        "\nthe `%s` file in a text editor."
        , g_PluginLoaderConfig_Filename
    );
    if (ImGui::Button("Open DLL's folder in File Explorer (has config)"))
    {
        system(("explorer \"" + GetThisDLLAbsolutePath().parent_path().string() + "\"").c_str());
    }
    ImGui::DrawEnumPicker("Mod menu hotkey", g_PluginLoaderConfig.hotkey_ToggleMenu.get(), ImGuiComboFlags_HeightLarge);
}

void DrawPluginListControls();
void DrawPluginsWhenMenuOpen();
void DrawPluginsEvenWhenMenuIsClosed();
void Base::ImGuiLayer_WhenMenuIsOpen()
{
    static bool enableDemoWindow = false;
    if (enableDemoWindow) {
        ImGui::ShowDemoWindow();
    }
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
    if (ImGuiCTX::Window _mainWindow{ "ACU Plugin Loader" })
    {
        DrawPluginsWhenMenuOpen();
        if (ImGuiCTX::TabBar _tabbar{ "MainWindowTabs" })
        {
            if (ImGuiCTX::Tab _mainTab{ "Main Tab" })
            {
                if (ImGuiCTX::WindowChild _{ "MainTabChild" })
                {
                    DrawPluginListControls();
                }
            }
            if (ImGuiCTX::Tab _extraoptions{ "Extra" })
            {
                if (ImGui::Button("Save Plugin Loader config"))
                {
                    PluginLoaderConfig::WriteToFile();
                }
                DrawModMenuControls();
                ImGui::Separator();
                ImGui::Checkbox("Show the text in the top left corner", &g_PluginLoaderConfig.imgui_showSuccessfulInjectionIndicator.get());
                ImGui::Separator();
                ImGui::Checkbox("Show ImGui Demo Window", &enableDemoWindow);
                ImGui::Separator();
                ImGui::Checkbox("Show developer options", &g_PluginLoaderConfig.developerOptions->isActive.get());
                if (g_PluginLoaderConfig.developerOptions->isActive)
                {
                    ImGui::Checkbox("Allow uninject the PluginLoader", &g_PluginLoaderConfig.developerOptions->canUninjectPluginLoader->isActive.get());
                    if (g_PluginLoaderConfig.developerOptions->canUninjectPluginLoader->isActive)
                    {
                        ImGuiCTX::Indent _ind;
                        ImGui::DrawEnumPicker(
                            "Hotkey: Uninject everything"
                            , g_PluginLoaderConfig.developerOptions->canUninjectPluginLoader->hotkey_UninjectPluginLoader.get()
                            , ImGuiComboFlags_HeightLarge);
                        if (ImGui::IsItemHovered(0))
                        {
                            ImGui::SetTooltip("You can use this to unload all the plugins and the plugin loader itself.");
                        }
                    }
                }
            }
            if (g_PluginLoaderConfig.developerOptions->isActive)
            {
                if (ImGuiCTX::Tab _typeInfosTab{ "DX11-BaseHook variables" })
                {
                    Base::ImGuiDrawBasehookDebug();
                }
            }
        }
    }
}
void EveryFrameBeforeGraphicsUpdate();
void Base::ImGuiLayer_EvenWhenMenuIsClosed()
{
    EveryFrameBeforeGraphicsUpdate();
    if (g_PluginLoaderConfig.imgui_showSuccessfulInjectionIndicator)
        DrawSuccessfulInjectionIndicatorOverlay();
    DrawPluginsEvenWhenMenuIsClosed();
}

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
            "Press %s to open ImGui menu, press %s to unload the mod."
            "\nThese hotkeys can be changed in the menu or in the config file (the default INSERT/END will still work)."
            "\nThis text can be disabled in the Extra tab of the menu. See `ACUFixes-readme.txt`."
            , enum_reflection<VirtualKeys>::GetString(g_PluginLoaderConfig.hotkey_ToggleMenu)
            , enum_reflection<VirtualKeys>::GetString(g_PluginLoaderConfig.hotkey_UnloadMod)
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
    ImGui::DrawEnumPicker("Unload mod hotkey", g_PluginLoaderConfig.hotkey_UnloadMod.get(), ImGuiComboFlags_HeightLarge);
}

void Base::ImGuiLayer_WhenMenuIsOpen()
{
    static bool enableDemoWindow = false;
    if (enableDemoWindow) {
        ImGui::ShowDemoWindow();
    }
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
    if (ImGuiCTX::Window _mainWindow{ "ACUFixes Mod Menu" })
    {
        if (ImGuiCTX::TabBar _tabbar{ "MainWindowTabs" })
        {
            if (ImGuiCTX::Tab _mainTab{ "Main Tab" })
            {
                if (ImGuiCTX::WindowChild _{ "MainTabChild" })
                {
                }
            }
            if (ImGuiCTX::Tab _extraoptions{ "Extra" })
            {
                DrawModMenuControls();
                ImGui::Separator();
                ImGui::Checkbox("Show the text in the top left corner", &g_PluginLoaderConfig.imgui_showSuccessfulInjectionIndicator.get());
                ImGui::Separator();
                ImGui::Checkbox("Show ImGui Demo Window", &enableDemoWindow);
            }
        }
    }
}
void Base::ImGuiLayer_EvenWhenMenuIsClosed()
{
    if (g_PluginLoaderConfig.imgui_showSuccessfulInjectionIndicator)
        DrawSuccessfulInjectionIndicatorOverlay();
}

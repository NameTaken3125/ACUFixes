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
            "\nThis text can be disabled in the Extra tab of the menu. See `ACUFixesAndPluginLoader-readme.txt`."
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
void DrawFontControls()
{
    if (ImGui::SliderFloat("Font size", &g_PluginLoaderConfig.fontSize.get(), 9.0f, 75.0f, "%.3f",
        ImGuiSliderFlags_AlwaysClamp))
    {
        Base::Fonts::SetFontSize(g_PluginLoaderConfig.fontSize);
    }
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
    DrawPluginsWhenMenuOpen();
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
    if (ImGuiCTX::Window _mainWindow{ "ACU Plugin Loader" })
    {
        if (ImGuiCTX::TabBar _tabbar{ "MainWindowTabs" })
        {
            if (ImGuiCTX::Tab _mainTab{ "Main Tab" })
            {
                DrawPluginListControls();
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
                ImGui::DrawEnumPicker(
                    "Hotkey: Console"
                    , g_PluginLoaderConfig.hotkey_ToggleConsole.get()
                    , ImGuiComboFlags_HeightLarge);
                ImGui::Separator();
                DrawFontControls();
                ImGui::Separator();
                ImGui::Checkbox("Show ImGui Demo Window", &enableDemoWindow);
                ImGui::Separator();
                ImGui::Checkbox("Show developer options", &g_PluginLoaderConfig.developerOptions->isActive.get());
                if (g_PluginLoaderConfig.developerOptions->isActive)
                {
                    ImGui::Checkbox("Allow uninject the PluginLoader", &g_PluginLoaderConfig.developerOptions->canUninjectPluginLoader->isActive.get());
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip(
                            "You aren't supposed to uninject the PluginLoader.\n"
                            "Doing so is likely to crash the game.\n"
                            "Furthermore, it's up to the plugins to decide\n"
                            "whether or not they can be safely uninjected.\n"
                            "If you're curious: currently the reason why such a crash is unavoidable\n"
                            "is my Animated Removable Hood Toggle. See more details in the code comments.\n"
                            "You can still try and uninject if you need to."
                        );
                        /*
                        Currently the only reason why I cannot safely uninject the PluginLoader
                        is my Animated Removable Hood Toggle. It works by extending the player's
                        (and all NPCs') Animation Graph and due to certain implementation details
                        I need to maintain the patches that at runtime adjust the indices of the last two
                        RTCP variables of the AtomGraph, the only two of the `entityref` type:
                            539. entityref SyncParticipant1; // 0x324db80/52747136
                            540. entityref SyncSubject; // 0xe35d816d/3814556013
                        Basically, I insert a new RTCP variable `int HoodControlValue` at index 539
                        and displace the two vars mentioned above: SyncParticipant1->540; SyncSubject->541
                        (because "entityrefs" are supposed to be the last ones).
                        So whenever a user of the patched AtomGraph (player or NPC)
                        refers to an "entityref" at index 539, it should be referring to index 540,
                        and this adjustment of the index is what I need to maintain at runtime.
                        A way to regain uninjectability would be to look through the AtomGraph dump
                        and patch every single reference to SyncSubject and SyncParticipant1.
                        A quick look at the dump shows 173 references to SyncParticipant1 and 129
                        to SyncSubject, and there most likely are more that I missed.
                        Yet another way would be to change the "adjust entityref index" patches
                        from C++ to assembly, and not deallocate them upon uninjection,
                        with some way to find "rediscover" these patches upon reinjection.
                        I'm going to declare this a very low priority feature and just accept that
                        the PluginLoader isn't supposed to be uninjected.
                        */
                    }
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
#include "ImGuiConsole.h"
void Base::ImGuiLayer_EvenWhenMenuIsClosed()
{
    EveryFrameBeforeGraphicsUpdate();
    if (g_PluginLoaderConfig.imgui_showSuccessfulInjectionIndicator
        && g_ConsoleMode == ConsoleMode::Hidden
        )
        DrawSuccessfulInjectionIndicatorOverlay();
    DrawConsoleIfVisible();
    DrawPluginsEvenWhenMenuIsClosed();
}

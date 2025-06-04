#include "pch.h"

#include "Common_Plugins/Common_PluginSide.h"
#include "ImGuiCTX.h"
#include "ImGuiConfigUtils.h"

#include "AssetOverrides/AssetOverrides.h"
#include "AssetOverrides/AssetOverridesConfig.h"

void DrawAssetOverridesInstructions();
void DrawAssetOverridesSettings();

class HacksContainer_AssetOverrides
{
public:
    AutoAssembleWrapper<AddVirtualForges> virtualForges;
    void DrawControls()
    {
        DrawAssetOverridesInstructions();
        ImGui::Separator();
        ImGui::DrawCheckboxForHack(virtualForges, "Enable Asset Overrides");
        ImGui::SameLine();
        ImGui::Checkbox("Enable on startup", &g_AssetOverridesConfig.enableOnStartup.get());
        ImGui::SameLine();
        if (ImGui::Button("Save config"))
        {
            WriteConfig();
        }
        ImGui::SameLine();
        if (ImGui::Button("Open DLL's folder in File Explorer"))
        {
            system(("explorer \"" + g_ThisDLLAbsoluteFilepath.parent_path().string() + "\"").c_str());
        }
        if (virtualForges.IsActive())
        {
            ImGui::Separator();
            DrawAssetOverridesSettings();
        }
        ImGui::Separator();
    }

    void ReadConfig()
    {
        virtualForges.Toggle(g_AssetOverridesConfig.enableOnStartup);
    }
    void WriteConfig()
    {
        AssetOverridesConfig::WriteToFile();
    }
};

std::optional<HacksContainer_AssetOverrides> g_HacksContainer_AssetOverrides;


void HacksContainer_AssetOverrides_DrawControls()
{
    if (g_HacksContainer_AssetOverrides)
    {
        g_HacksContainer_AssetOverrides->DrawControls();
    }
}

void AssetOverrides_CodePatches_Start()
{
    g_HacksContainer_AssetOverrides.emplace();
    g_HacksContainer_AssetOverrides->ReadConfig();
}

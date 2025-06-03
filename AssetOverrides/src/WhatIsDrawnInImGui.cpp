#include "pch.h"

#include "vmath/vmath.h"
#include <vmath/vmath_extra.h>

#include "ImGuiCTX.h"

#include "ACU/ACUGetSingletons.h"
#include "ACU/Entity.h"
#include "ACU/RenderValuesHolder.h"
#include "ACU/ManagedPtrs/ManagedPtrs.h"



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


#include "ImGuiConfigUtils.h"
#include "Common_Plugins/Common_PluginSide.h"
void DrawModMenuControls()
{
    if (ImGui::Button("Open DLL's folder in File Explorer (has config)"))
    {
        system(("explorer \"" + g_ThisDLLAbsoluteFilepath.parent_path().string() + "\"").c_str());
    }
}

void DrawImGui3DMatricesDebug();
void RequestUnloadThisPlugin();
void DrawAnimationExperiments();
void DrawHacksControls_DevExtras();
void HacksContainer_AssetOverrides_DrawControls();
#include "Handles.h"
void ImGuiLayer_WhenMenuIsOpen()
{
#ifdef _DEBUG
    void RequestUnloadThisPlugin(); if (ImGui::Button("Unload this plugin")) RequestUnloadThisPlugin();
    ImGui::Separator();
#endif // _DEBUG
        if (ImGuiCTX::TabBar _tabbar{ "MainWindowTabs" })
        {
            if (ImGuiCTX::Tab _mainTab{ "Asset Overrides" })
            {
                HacksContainer_AssetOverrides_DrawControls();
            }
            if (ImGuiCTX::Tab _extraoptions{ "Extra" })
            {
                DrawModMenuControls();
                ImGui::Separator();
                if (ImGui::Button("Unload this plugin"))
                {
                    RequestUnloadThisPlugin();
                }
            }
        }
}

void ImGuiLayer_EvenWhenMenuIsClosed()
{}

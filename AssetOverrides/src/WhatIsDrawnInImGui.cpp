#include "pch.h"

#include "ImGuiCTX.h"

#include "Common_Plugins/Common_PluginSide.h"

void RequestUnloadThisPlugin();
void HacksContainer_AssetOverrides_DrawControls();
void ImGuiLayer_WhenMenuIsOpen()
{
#ifdef _DEBUG
    if (ImGui::Button("Unload this plugin")) RequestUnloadThisPlugin();
    ImGui::Separator();
#endif // _DEBUG
    HacksContainer_AssetOverrides_DrawControls();
}

void ImGuiLayer_EvenWhenMenuIsClosed()
{}

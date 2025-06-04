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
    //if (ImGuiCTX::TabBar _tabbar{ "MainWindowTabs" })
    //{
    //    if (ImGuiCTX::Tab _mainTab{ "Asset Overrides" })
    //    {
            HacksContainer_AssetOverrides_DrawControls();
    //    }
    //    if (ImGuiCTX::Tab _extraoptions{ "Extra" })
    //    {
    //        DrawModMenuControls();
    //        ImGui::Separator();
    //        if (ImGui::Button("Unload this plugin"))
    //        {
    //            RequestUnloadThisPlugin();
    //        }
    //    }
    //}
}

void ImGuiLayer_EvenWhenMenuIsClosed()
{}

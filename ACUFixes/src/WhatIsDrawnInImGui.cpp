#include "pch.h"

#include "vmath/vmath.h"
#include <vmath/vmath_extra.h>

#include "ImGuiCTX.h"
#include "ImGui3D/ImGui3DRenderer.h"

#include "ACU/ACUGetSingletons.h"
#include "ACU/Entity.h"
#include "ACU/RenderValuesHolder.h"
#include "ACU/ManagedPtrs/ManagedPtrs.h"

Vector3f g_VisualizedDebugDirection;
void VisualizeLocationFromClipboard()
{
    Vector3f visualizedLoc = ParseVector3fFromClipboard().value_or(Vector3f());
    ImGui3D::DrawLocationNamed(visualizedLoc, "Vizualized Loc");
}


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
void VisualizeCurrentPlayerLocation()
{
    Entity* player = ACU::GetPlayer();
    Vector3f loc = player ? player->GetPosition() : Vector3f();
    ImGui3D::DrawLocationNamed(loc, "Player");
}
void VisualizeDirectionFromClipboard()
{
    g_VisualizedDebugDirection = ParseVector3fFromClipboard().value_or(g_VisualizedDebugDirection);
}
void DrawHacksControls();
void TypeInfoSystemTests();



void DrawBuiltinDebugCommands();
void DrawPlayerVisualsControls();
void DrawWeatherControls();
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
#include "MainConfig.h"
#include "Handles.h"
void ImGuiLayer_WhenMenuIsOpen()
{
#ifdef _DEBUG
    void RequestUnloadThisPlugin(); if (ImGui::Button("Unload this plugin")) RequestUnloadThisPlugin();
    ImGui::Separator();
#endif // _DEBUG
        if (ImGuiCTX::TabBar _tabbar{ "MainWindowTabs" })
        {
            if (ImGuiCTX::Tab _mainTab{ "Main Tab" })
            {
                DrawHacksControls();
            }
            if (ImGuiCTX::Tab _mainTab{ "Weather" })
            {
                DrawWeatherControls();
            }
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
                ImGui::Separator();
                ImGui::Checkbox("Show development experiments", &g_Config.developmentExtras->showDevelopmentExtras.get());
                if (ImGui::IsItemHovered(0))
                {
                    ImGui::SetTooltip("These contain my experiments and nothing that improves the gameplay.");
                }
                if (g_Config.developmentExtras->showDevelopmentExtras)
                {
                    ImGui::Checkbox("Draw 3D markers", &g_Config.developmentExtras->show3DMarkersIfDevelopmentExtrasAreEnabled.get());
                }
            }
            if (!g_Config.developmentExtras->showDevelopmentExtras) return;
            auto DrawDevExtrasTabs = []()
                {
                    if (ImGuiCTX::Tab _3dMarkersTab{ "3D Markers" })
                    {
                        ImGui::Checkbox("Draw 3D markers", &g_Config.developmentExtras->show3DMarkersIfDevelopmentExtrasAreEnabled.get());
                        ImGui3D::DrawPersistent3DMarkersControls();
                        if (ImGui::Button("Visualize location from clipboard"))
                        {
                            VisualizeLocationFromClipboard();
                        }
                        if (ImGui::Button("Visualize direction from clipboard"))
                        {
                            VisualizeDirectionFromClipboard();
                        }
                        if (ImGui::Button("Visualize current Player Location"))
                        {
                            VisualizeCurrentPlayerLocation();
                        }
                        if (ImGui::CollapsingHeader("View-projection matrices debugging"))
                        {
                            DrawImGui3DMatricesDebug();
                        }
                    }
                    if (ImGuiCTX::Tab _tab_parkourDebug{ "Movement debug" })
                    {
                        DrawHacksControls_DevExtras();
                    }
                    if (ImGuiCTX::Tab _{ "Player's Visuals" })
                    {
                        DrawPlayerVisualsControls();
                    }
                    if (ImGuiCTX::Tab _mainTab{ "Animtools" })
                    {
                        if (ImGui::Button("Go to top of Bastille"))
                        {
                            if (Entity* player = ACU::GetPlayer())
                            {
                                player->GetPosition() = Vector3f(1200.23f, 150.99f, 39.00f);
                            }
                        }
                        DrawAnimationExperiments();
                    }
                    if (ImGuiCTX::Tab _typeInfosTab{ "TypeInfos" })
                    {
                        TypeInfoSystemTests();
                    }
                    if (ImGuiCTX::Tab _typeInfosTab{ "Builtin Commands" })
                    {
                        if (ImGuiCTX::WindowChild _{ "DebugCommands" })
                        {
                            DrawBuiltinDebugCommands();
                        }
                    }
                    if (ImGuiCTX::Tab _tab_handles{ "Handles" })
                    {
                        extern const char* g_HandlesMapFilename;
                        ImGui::Text(
                            "Some of the development extra features show handles of files\n"
                            "e.g. Animation log, Animation Graph dump, entity Visual components etc.\n"
                            "If you have the \"%s\" file\n"
                            "(which should be available in the Releases section of ACUFixes github page)\n"
                            "present in the plugin's directory,\n"
                            "then the names of the files that correspond to these handles\n"
                            "will also be shown (if recognized).\n"
                            "For example, if the mentioned file is present, then you should see\n"
                            "64839213519 => \n"
                            "turn into\n"
                            "64839213519 => ACU_Paris\\ACU_Paris.World"
                            , g_HandlesMapFilename
                        );
                        static uint64 handleToSearch = 64839213519; // => ACU_Paris\ACU_Paris.World
                        ImGui::InputScalar("Search handle (dec)", ImGuiDataType_U64, &handleToSearch, 0, 0, "%llu", ImGuiInputTextFlags_CharsDecimal);
                        ImGui::InputScalar("Search handle (hex)", ImGuiDataType_U64, &handleToSearch, 0, 0, "%llX", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
                        ImGui::Text(
                            "%llu => %s"
                            , handleToSearch
                            , ACU::Handles::HandleToText(handleToSearch).c_str()
                        );
                        ACU::WeakRef<ManagedObject> weakRef(handleToSearch);
                        static ImGuiTextBuffer buf;
                        buf.clear();
                        buf.appendf(
                            "Address:     %llX\n"
                            "SharedBlock: %llX\n"
                            "StrongRefs:  %u\n"
                            "WeakRefs:    %u"
                            , weakRef.GetSharedBlock().GetPtr()
                            , &weakRef.GetSharedBlock()
                            , weakRef.GetSharedBlock().GetRefcountStrong()
                            , weakRef.GetSharedBlock().weakRefCount
                        );
                        ImGui::Text(buf.c_str());
                        ImGui::CopyToClipboardOnClick(buf.c_str());
                    }
                };
            if (ImGuiCTX::Tab _tab_devExtras{ "Dev extras" })
            {
                if (ImGuiCTX::TabBar _tabbar_withinDevExtrasTab{ "DevExtrasTabs" })
                {
                    DrawDevExtrasTabs();
                }
            }
        }
}

ImGui3D::World2ScreenParams CalculateWorld2ScreenParametersForCurrentFrame();
void DoSlowMotionTrick();
void DoManualHoodControls();
void ShowHumanStatesLogIfNeeded();
#include "AnimationTools/MyAnimationPlayer.h"
void ImGuiLayer_EvenWhenMenuIsClosed()
{
    g_MyAnimationPlayer.UpdateAnimations();
    ShowHumanStatesLogIfNeeded();
    DoSlowMotionTrick();
    DoManualHoodControls();
    bool drawImGui3D =
        g_Config.developmentExtras->showDevelopmentExtras
        && g_Config.developmentExtras->show3DMarkersIfDevelopmentExtrasAreEnabled;
    if (drawImGui3D)
        ImGui3D::Draw3DLayer(CalculateWorld2ScreenParametersForCurrentFrame());
}

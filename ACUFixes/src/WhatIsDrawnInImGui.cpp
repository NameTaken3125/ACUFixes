#include "pch.h"

#include "vmath/vmath.h"
#include <vmath/vmath_extra.h>

#include "ImGuiCTX.h"
#include "ImGui3D/ImGui3DRenderer.h"

#include "ACU/ACUGetSingletons.h"
#include "ACU/Entity.h"
#include "ACU/RenderValuesHolder.h"

Vector3f g_VisualizedDebugDirection;
void VisualizeLocationFromClipboard()
{
    Vector3f visualizedLoc = ParseVector3fFromClipboard().value_or(Vector3f());
    ImGui3D::DrawLocationNamed(visualizedLoc, "Vizualized Loc");
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
#include "MainConfig.h"
bool g_showDevExtraOptions = false;
bool g_DrawImGui3DifDevExtrasEnabled = true;
void ImGuiLayer_WhenMenuIsOpen()
{
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
        if (ImGuiCTX::TabBar _tabbar{ "MainWindowTabs" })
        {
            if (ImGuiCTX::Tab _mainTab{ "Main Tab" })
            {
                if (ImGuiCTX::WindowChild _{ "MainTabChild", ImVec2(0, 0), true })
                {
                    DrawHacksControls();
                }
            }
            if (ImGuiCTX::Tab _mainTab{ "Weather" })
            {
                if (ImGuiCTX::WindowChild _{ "WeatherTabChild", ImVec2(0, 0), true })
                {
                    DrawWeatherControls();
                }
            }
            if (ImGuiCTX::Tab _mainTab{ "Animtools" })
            {
                if (ImGuiCTX::WindowChild _{ "animschild", ImVec2(0, 0), true })
                {
                    if (ImGui::Button("Go to top of Bastille"))
                    {
                        if (Entity* player = ACU::GetPlayer())
                        {
                            player->GetPosition() = Vector3f(1200.23f, 150.99f, 39.00f);
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Unload this plugin"))
                    {
                        RequestUnloadThisPlugin();
                    }
                    DrawAnimationExperiments();
                }
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
                ImGui::Checkbox("Show development experiments", &g_showDevExtraOptions);
                if (ImGui::IsItemHovered(0))
                {
                    ImGui::SetTooltip("These contain my experiments and nothing that improves the gameplay.");
                }
                if (g_showDevExtraOptions)
                {
                    ImGui::Checkbox("Draw 3D markers", &g_DrawImGui3DifDevExtrasEnabled);
                }
            }
            if (g_showDevExtraOptions)
            {
            if (ImGuiCTX::Tab _3dMarkersTab{ "3D Markers" })
            {
                ImGui::Checkbox("Draw 3D markers", &g_DrawImGui3DifDevExtrasEnabled);
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
            if (ImGuiCTX::Tab _{ "Player's Visuals" })
            {
                DrawPlayerVisualsControls();
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
            }
        }
}

ImGui3D::World2ScreenParams CalculateWorld2ScreenParametersForCurrentFrame();
void DoSlowMotionTrick();
void DoManualHoodControls();
#include "AnimationTools/MyAnimationPlayer.h"
void ImGuiLayer_EvenWhenMenuIsClosed()
{
    g_MyAnimationPlayer.UpdateAnimations();
    DoSlowMotionTrick();
    DoManualHoodControls();
    bool drawImGui3D = g_showDevExtraOptions && g_DrawImGui3DifDevExtrasEnabled;
    if (drawImGui3D)
        ImGui3D::Draw3DLayer(CalculateWorld2ScreenParametersForCurrentFrame());
}

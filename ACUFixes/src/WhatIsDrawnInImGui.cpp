#include "pch.h"

#include "base.h"
#include "vmath/vmath.h"
#include <vmath/vmath_extra.h>

#include "ImGuiCTX.h"
#include "ImGui3D.h"
#include "ImGui3DRenderer.h"

#include "ACU/ACUGetSingletons.h"
#include "ACU/Entity.h"
#include "ACU/RenderValuesHolder.h"

Matrix4f gameMatView;
Matrix4f gameMatProj;
void ImGuiPrintMatrix(const Matrix4f& mat)
{
    for (size_t i = 0; i < 4; i++)
    {
        ImGui::Text("%f %f %f %f:"
            , mat.data[i * 4]
            , mat.data[i * 4 + 1]
            , mat.data[i * 4 + 2]
            , mat.data[i * 4 + 3]
        );
    }
}
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

Matrix4f MakeSimpleDebugTransform(const Vector3f& position)
{
    return Matrix4f::createTranslation(position.x, position.y, position.z) * Matrix4f::createScale(0.1f, 0.1f, 0.1f);
}



#include "MainConfig.h"
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
            , enum_reflection<VirtualKeys>::GetString(g_Config.hotkey_ToggleMenu)
            , enum_reflection<VirtualKeys>::GetString(g_Config.hotkey_UnloadMod)
            );
    }
    ImGui::End();
}
void SetProjMatrix(Matrix4f& matOut)
{
    matOut = RenderValuesHolder::GetSingleton()->matProjection_mb;
}
// The stupid game has the camera matrix all rotated around.
// Why don't you stop rotating your matrices, game?
void SetCorrectViewMatrix(Matrix4f& matOut)
{
    Matrix4f cameraMat = RenderValuesHolder::GetSingleton()->cameraTransform;
    cameraMat = cameraMat * Matrix4f::createRotationAroundAxis(-90, 0, 0);
    matOut = cameraMat.inverse();
}
void ImGui3D::CalculateViewProjectionForCurrentFrame(Matrix4f& viewProjOut)
{
    SetProjMatrix(gameMatProj);
    SetCorrectViewMatrix(gameMatView);
    viewProjOut = gameMatProj * gameMatView;
}

Entity* FindHighlightedNPC();
std::optional<Vector3f> GetDisguiseTargetPosition();
void ImGui3D::WhatIsActuallyDrawnForFrame()
{
    // On a pile of junk next to the artiste.
    static Vector3f testPosition{ 127.82f, 704.28f, 1.06f };
    ImGui3D::DrawWireModel(ImGui3D::GetArrowModel(), testPosition);
    std::optional<Vector3f> currentDisguiseTargetPos = GetDisguiseTargetPosition();
    if (currentDisguiseTargetPos)
    {
        ImGui3D::DrawWireModel(ImGui3D::GetCrossModel(), *currentDisguiseTargetPos);
    }
    Entity* player = ACU::GetPlayer();

    static ImGui3D::ImGuiWireModel grid5_model = ImGui3D::GenerateGrid(5, 2);

    if (player)
    {
        Matrix4f debugDirectionTransform;
        debugDirectionTransform.setRotation(MakeRotationAlignZWithVector(g_VisualizedDebugDirection));
        debugDirectionTransform = Matrix4f::createTranslation(player->GetPosition()) * debugDirectionTransform;
        ImGui3D::DrawWireModelTransform(ImGui3D::GetArrowModel(), debugDirectionTransform);
        ImGui3D::DrawWireModelTransform(ImGui3D::GetArrowModel(), player->GetTransform());
        ImGui3D::DrawWireModelTransform(grid5_model, player->GetTransform());

        Entity* rangedWeaponTargetNPC = FindHighlightedNPC();
        if (rangedWeaponTargetNPC)
        {
            ImGui3D::DrawWireModelTransform(grid5_model, rangedWeaponTargetNPC->GetTransform());
        }
    }
    ImGui3D::DrawMarkers();
}
void DrawBuiltinDebugCommands();
void DrawPlayerVisualsControls();
void DrawWeatherControls();
#include "Enum_VirtualKeys.h"
#include "ImGuiConfigUtils.h"
std::filesystem::path& GetThisDLLAbsolutePath();
void DrawModMenuControls()
{
    ImGui::Text(
        "Mod menu hotkeys."
        "\nYou can also change the hotkeys by manually editing"
        "\nthe `acufixes-config.json` file in a text editor."
    );
    if (ImGui::Button("Open DLL's folder in File Explorer (has config)"))
    {
        system(("explorer \"" + GetThisDLLAbsolutePath().parent_path().string() + "\"").c_str());
    }
    ImGui::DrawEnumPicker("Mod menu hotkey", g_Config.hotkey_ToggleMenu.get(), ImGuiComboFlags_HeightLarge);
    ImGui::DrawEnumPicker("Unload mod hotkey", g_Config.hotkey_UnloadMod.get(), ImGuiComboFlags_HeightLarge);
}

#include "MainConfig.h"
bool g_showDevExtraOptions = false;
bool g_DrawImGui3DifDevExtrasEnabled = true;
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
                    DrawHacksControls();
                }
            }
            if (ImGuiCTX::Tab _mainTab{ "Weather" })
            {
                if (ImGuiCTX::WindowChild _{ "WeatherTabChild" })
                {
                    DrawWeatherControls();
                }
            }
            if (ImGuiCTX::Tab _extraoptions{ "Extra" })
            {
                DrawModMenuControls();
                ImGui::Separator();
                ImGui::Checkbox("Show the text in the top left corner", &g_Config.imgui_showSuccessfulInjectionIndicator.get());
                ImGui::Separator();
                ImGui::Checkbox("Show development experiments", &g_showDevExtraOptions);
                if (ImGui::IsItemHovered(0))
                {
                    ImGui::SetTooltip("These contain my experiments and nothing that improves the gameplay.");
                }
                if (g_showDevExtraOptions)
                {
                    ImGui::Checkbox("Draw 3D markers", &g_DrawImGui3DifDevExtrasEnabled);
                    ImGui::Separator();
                    ImGui::Checkbox("Show ImGui Demo Window", &enableDemoWindow);
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
                    ImGui::Text("View matrix:");
                    ImGuiPrintMatrix(gameMatView);
                    ImGui::Text("Projection matrix:");
                    ImGuiPrintMatrix(gameMatProj);
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
            if (ImGuiCTX::Tab _typeInfosTab{ "DX11-BaseHook variables" })
            {
                Base::ImGuiDrawBasehookDebug();
            }
            }
        }
    }
}
void DoSlowMotionTrick();
void DoManualHoodControls();
void Base::ImGuiLayer_EvenWhenMenuIsClosed()
{
    DoSlowMotionTrick();
    DoManualHoodControls();
    bool drawImGui3D = g_showDevExtraOptions && g_DrawImGui3DifDevExtrasEnabled;
    if (drawImGui3D)
        ImGui3D::DrawStuff();
    if (g_Config.imgui_showSuccessfulInjectionIndicator)
        DrawSuccessfulInjectionIndicatorOverlay();
}
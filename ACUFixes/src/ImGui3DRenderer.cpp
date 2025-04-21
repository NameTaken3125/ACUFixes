#include "pch.h"

#include "ImGui3D.h"
#include "ImGui3DRenderer.h"
#include "ImGui3DCustomDraw.h"

namespace ImGui3D
{
extern ImDrawList* g_DrawList;
extern World2ScreenParams g_World2ScreenParams;

void ImGui3D::Draw3DLayer(const ImGui3D::World2ScreenParams& w2sParams)
{
    ImGui3D::g_World2ScreenParams = w2sParams;
    // Create a window the size of the screen with a transparent background.
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
    ImGuiWindowFlags imgu3dWindowFlags = 0;
    imgu3dWindowFlags |= ImGuiWindowFlags_NoBackground;
    imgu3dWindowFlags |= ImGuiWindowFlags_NoDecoration;
    imgu3dWindowFlags |= ImGuiWindowFlags_NoMove;
    imgu3dWindowFlags |= ImGuiWindowFlags_NoResize;
    imgu3dWindowFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
    imgu3dWindowFlags |= ImGuiWindowFlags_NoNav;
    imgu3dWindowFlags |= ImGuiWindowFlags_NoInputs;

    if (ImGui::Begin("ImGui3D", 0, imgu3dWindowFlags))
    {
        ImGui3D::g_DrawList = ImGui::GetWindowDrawList();
        ImGui3D::DrawMarkers();
        ImGui3D::CustomDraw::DrawAllCustom();
    }
    ImGui::End();
}
} // namespace ImGui3D

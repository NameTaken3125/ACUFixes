#include "pch.h"

#include "ImGui3D.h"
#include "ImGui3DRenderer.h"


void ImGui3D::DrawStuff()
{
    CalculateViewProjectionForCurrentFrame(ImGui3D::g_ViewProjection);
    // ImGuizmo only draws within the bounds of an ImGui Window.
    // Create a window the size of the screen with a transparent background.
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    ImGui3D::g_WindowSize = (Vector2f&)windowSize;
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
        WhatIsActuallyDrawnForFrame();
    }
    ImGui::End();
}
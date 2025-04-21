#include "pch.h"

#include "ACU/Entity.h"
#include "ACU/ACUGetSingletons.h"
#include "vmath/vmath_extra.h"
#include "MainConfig.h"

#include "ImGui3D.h"
#include "ImGui3DCustomDraw.h"


#include "ACU/Letterbox.h"
#include "ACU/RenderValuesHolder.h"
static void CalculateViewProjectionForCurrentFrame(Matrix4f& viewProjOut)
{
    auto* renderValues = RenderValuesHolder::GetSingleton();
    if (!renderValues) return;
    viewProjOut = renderValues->matViewProjection;
    return;

    // Below is the old method of creating ViewProjection from Camera and Projection matrices.
    // Left for posterity, in case I need to repeat this in a different game,
    // because the latter two matrices are easier to find.

    Matrix4f& gameMatProj = renderValues->matProjection;
    // The stupid game has the camera matrix all rotated around.
    // Why don't you stop rotating your matrices, game?
    Matrix4f cameraMat = renderValues->cameraTransform;
    cameraMat = cameraMat * Matrix4f::createRotationAroundAxis(-90, 0, 0);
    Matrix4f gameMatView = cameraMat.inverse();
    viewProjOut = gameMatProj * gameMatView;
}
void CalculateViewportForCurrentFrame(Vector2f& topLeftOut, Vector2f& bottomRightOut)
{
    auto* renderValues = RenderValuesHolder::GetSingleton();
    if (!renderValues) return;
    auto* letterboxAdjustment = HasRenderTargetSizeAccountingForLetterbox::GetSingleton();
    if (!letterboxAdjustment) return;
    Vector2f windowSize = (const Vector2f&)ImGui::GetIO().DisplaySize;
    Vector2f& windowSizeMinusLetterbox = letterboxAdjustment->displaySizeWithoutLetterbox;
    const float letterBoxOneSideStripSize = (windowSize.y - windowSizeMinusLetterbox.y) / 2;
    topLeftOut = { 0, letterBoxOneSideStripSize };
    bottomRightOut = { windowSize.x, windowSize.y - letterBoxOneSideStripSize };
}
ImGui3D::World2ScreenParams CalculateWorld2ScreenParametersForCurrentFrame()
{
    ImGui3D::World2ScreenParams result;
    CalculateViewProjectionForCurrentFrame(result.m_ViewProjection);
    CalculateViewportForCurrentFrame(result.m_ViewportTopLeft, result.m_ViewportBottomRight);
    return result;
}
void ImGuiPrintMatrix(const Matrix4f& mat)
{
    for (size_t i = 0; i < 4; i++)
    {
        ImGui::Text("%9.3f %9.3f %9.3f %9.3f:" // A total of 9 characters per number, leftpadded with spaces, e.g. at most "-1234.567"
            , mat.data[i * 4]
            , mat.data[i * 4 + 1]
            , mat.data[i * 4 + 2]
            , mat.data[i * 4 + 3]
        );
    }
}
namespace ImGui3D {
extern World2ScreenParams g_World2ScreenParams;
}
void DrawImGui3DMatricesDebug()
{
    auto* renderValues = RenderValuesHolder::GetSingleton();
    if (!renderValues) return;
    Matrix4f& gameMatProj = renderValues->matProjection;
    // The stupid game has the camera matrix all rotated around.
    // Why don't you stop rotating your matrices, game?
    Matrix4f cameraMat = renderValues->cameraTransform;
    cameraMat = cameraMat * Matrix4f::createRotationAroundAxis(-90, 0, 0);
    Matrix4f gameMatView = cameraMat.inverse();
    ImGui::Text("View matrix:");
    ImGuiPrintMatrix(gameMatView);
    ImGui::Text("Projection matrix:");
    ImGuiPrintMatrix(gameMatProj);
    ImGui::Text("ViewProjection matrix:");
    ImGuiPrintMatrix(ImGui3D::g_World2ScreenParams.m_ViewProjection);
}




Entity* FindHighlightedNPC();
std::optional<Vector3f> GetDisguiseTargetPosition();
extern Vector3f g_VisualizedDebugDirection;

void WhatIsActuallyDrawnForFrame()
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
        //ImGui3D::DrawWireModelTransform(ImGui3D::GetArrowModel(), player->GetTransform());
        //ImGui3D::DrawWireModelTransform(grid5_model, player->GetTransform());

        Entity* rangedWeaponTargetNPC = FindHighlightedNPC();
        if (rangedWeaponTargetNPC)
        {
            ImGui3D::DrawWireModelTransform(grid5_model, rangedWeaponTargetNPC->GetTransform());
        }
    }
    ImGui3D::DrawMarkers();
    ImGui3D::CustomDraw::DrawAllCustom();
}

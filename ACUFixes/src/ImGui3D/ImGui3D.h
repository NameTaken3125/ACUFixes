#pragma once

#include <optional>
#include "vmath/vmath.h"

struct ImDrawList;

namespace ImGui3D
{
/*
The matrices need to be oriented the following way:
Projection:
    1.407  0.000 0.000  0.000
    0.000  2.502 0.000  0.000
    0.000  0.000 -1.00  -1.00
    0.000  0.000 -0.10  0.000
View:
    0.89846 -0.1362 0.4173 0.000
    0.43904 0.27958 -0.853 0.000
    -0.0003 0.95039 0.3110 0.000
    -422.47 -181.47 542.89 1.000
Therefore, the View-Projection Matrix is:
    1.26492 -0.3411 -0.4173 -0.4173
    0.61812 0.69977 0.85387 0.85385
    -0.0004 2.37874 -0.3110 -0.3110
    -594.78 -454.20 -543.00 -542.89
(View Matrix = inverted Camera Transform Matrix, which in this case looks like the following:)
    0.898 0.4390 -0.00 0.000
    -0.13 0.2795 0.950 0.000
    0.417 -0.853 0.311 0.000
    128.2 699.77 3.475 1.000

Finding the correct Camera (and thus the View) Matrix can be tricky.
In the case of AC Unity, I found the camera transform matrix constructed in game,
but to use in-world rendering it required adjustment.
Specifically, it turned out:
    Correct View Matrix == (Camera Transform Matrix * {X axis rotation of -90 degrees})^-1.
So next time if the found View rotation doesn't result in successful rendering, try to
pre-rotate the View various degrees around various axes,
as well as apply some Inverse and Transpose operations
in case a game uses differently oriented matrices.

Viewport size:
If the game window is fullscreen, then
    m_ViewportTopLeft = Vector(0, 0); and
    m_ViewportBottomRight = (const Vector2f&)ImGui::GetIO().DisplaySize;
If for example the window has a letterbox (like ACU does
when the graphics option Stretching is off), then:
    m_ViewportTopLeft = Vector2f(0, letterboxWidth); and
    m_ViewportBottomRight = Vector2f(imguiDisplaySize.x, imguiDisplaySize.y - letterboxWidth);
*/
struct World2ScreenParams
{
    Matrix4f m_ViewProjection;
    Vector2f m_ViewportTopLeft;
    Vector2f m_ViewportBottomRight;
};
struct ModelEdge
{
    short idx1, idx2;
    ImU32 color;
};
class ImGuiWireModel
{
public:
    ImGuiWireModel(const std::vector<Vector3f>& points, const std::vector<ModelEdge>& edges)
        : points(points)
        , edges(edges)
        , worldPoints{ points.size() }
    {}
    std::vector<Vector3f> points;
    std::vector<ModelEdge> edges;
    mutable std::vector<Vector3f> worldPoints;
};
enum Axis { X = 0, Y = 1, Z = 2 };
static ImU32 axisColors[3] = {
    IM_COL32(255, 128, 128, 255), // x
    IM_COL32(128, 255, 128, 255), // y
    IM_COL32(255, 0, 0, 255), // z
};
ImGuiWireModel GenerateGrid(int howManyPointsOnSide = 5, float sideLength = 1);
ImGuiWireModel& GetArrowModel();
ImGuiWireModel& GetCrossModel();
ImGuiWireModel& GetModel_CubePlusMinus1();  // The points are: [-1, -1, -1], [1, 1, 1], [-1, -1, 1]...
ImGuiWireModel& GetModel_Cube01();          // The points are: [0, 0, 0], [1, 1, 1], [0, 0, 1]...


void DrawWireModel(const ImGui3D::ImGuiWireModel& model, const Vector3f& position, float thicknessMultiplier = 1, float scaleMultiplier = 1.0f, std::optional<ImU32> overrideColor = {});
void DrawWireModelTransform(const ImGui3D::ImGuiWireModel& model, const Matrix4f& transform, float thicknessMultiplier = 1, std::optional<ImU32> overrideColor = {});
void DrawWireModelOnce(const ImGui3D::ImGuiWireModel& model, const Matrix4f& transform);

void DrawLocationOnce(const Vector3f& location, float scaleMultiplier = 1.0f);
void DrawLocationAndPersist(const Vector3f& location);
void DrawLocationNamed(const Vector3f& location, const std::string& name, float scaleMultiplier = 1.0f);

void DrawLineOnce(const Vector3f& from_, const Vector3f& to_, float thicknessMultiplier = 1.0f, ImU32 color = axisColors[Axis::Z]);

void DrawMarkers();
void DrawPersistent3DMarkersControls();
}


std::optional<Vector3f> ParseVector3fFromClipboard();

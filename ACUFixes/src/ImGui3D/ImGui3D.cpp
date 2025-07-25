#include "pch.h"

#include "ImGui3D.h"
#include "ImGuiCTX.h"
#include "ImGui3DCustomDraw.h"


namespace ImGui3D {
World2ScreenParams g_World2ScreenParams;
ImDrawList* g_DrawList = nullptr;


// Point is visible <=> z-coordinate >= 0 after world-view-projection is applied;
std::optional<Vector2f> World2Screen(const Vector3f& ptWorld)
{
    const Matrix4f& gameMatViewProj = g_World2ScreenParams.m_ViewProjection;
    Vector4f ptTransformed = gameMatViewProj * Vector4f{ ptWorld, 1 };
    // Point is visible <=> z-coordinate >= 0 after world-view-projection is applied;
    if (ptTransformed.z < 0) { return {}; }
    Vector2f ptNormalizedCoords = ptTransformed.xy() / ptTransformed.w; // Visible points are in range [-1,1]x[-1,1]
    ptNormalizedCoords.y *= -1;      // Flip Y for ImGui.
    const Vector2f ptUV = (ptNormalizedCoords + Vector2f{ 1, 1 }) / 2;    // Visible points are in range [0,1]x[0,1]
    const Vector2f viewportSize = g_World2ScreenParams.m_ViewportBottomRight - g_World2ScreenParams.m_ViewportTopLeft;
    const Vector2f ptScreenSpace = g_World2ScreenParams.m_ViewportTopLeft + ptUV * viewportSize;
    return ptScreenSpace;
};
const float g_WireModelDefaultThickness = 2;
float g_WireModelGlobalSizeMultiplier = 1;
void DrawWireModel(const ImGuiWireModel& model, const Vector3f& position, float thicknessMultiplier, float scaleMultiplier, std::optional<ImU32> overrideColor)
{
    for (size_t i = 0; i < model.points.size(); i++)
    {
        model.worldPoints[i] = position + model.points[i] * scaleMultiplier * g_WireModelGlobalSizeMultiplier;
    }
    for (const ModelEdge& e : model.edges)
    {
        Vector3f lineStart_world = model.worldPoints[e.idx1];
        Vector3f lineEnd_world = model.worldPoints[e.idx2];
        std::optional<Vector2f> lineStart_window = World2Screen(lineStart_world); if (!lineStart_window) { continue; }
        std::optional<Vector2f> lineEnd_window = World2Screen(lineEnd_world); if (!lineEnd_window) { continue; }
        g_DrawList->AddLine((ImVec2&)lineStart_window.value(), (ImVec2&)lineEnd_window.value(), overrideColor ? *overrideColor : e.color, g_WireModelDefaultThickness * thicknessMultiplier);
    }
}
void DrawWireModelTransform(const ImGuiWireModel& model, const Matrix4f& transform, float thicknessMultiplier)
{
    for (size_t i = 0; i < model.points.size(); i++)
    {
        model.worldPoints[i] = (transform * Vector4f{ model.points[i], 1 }).xyz();
    }
    for (const ModelEdge& e : model.edges)
    {
        Vector3f lineStart_world = model.worldPoints[e.idx1];
        Vector3f lineEnd_world = model.worldPoints[e.idx2];
        std::optional<Vector2f> lineStart_window = World2Screen(lineStart_world); if (!lineStart_window) { continue; }
        std::optional<Vector2f> lineEnd_window = World2Screen(lineEnd_world); if (!lineEnd_window) { continue; }
        g_DrawList->AddLine((ImVec2&)lineStart_window.value(), (ImVec2&)lineEnd_window.value(), e.color, g_WireModelDefaultThickness * thicknessMultiplier);
    }
}
ImGuiWireModel GenerateGrid(int howManyPointsOnSide, float sideLength)
{
    /*
    Assume howManyPointsOnSide == 5, then:
    Generate vertices in the following order:
    5   6   7   8   9
    12             15
    11             14
    10             13
    0   1   2   3   4
    Then generate edges: first all 5 vertical lines:
    0 5
    1 6
    2 7
    3 8
    4 9
    then all horizontal lines except Topmost and Bottommost:
    10 13
    11 14
    12 15
    then Topmost and Bottommost:
    0 4
    5 9
    */
    if (howManyPointsOnSide < 2) { howManyPointsOnSide = 2; }
    if (sideLength <= 0) { sideLength = 1; }
    std::vector<Vector3f> points;
    points.resize((howManyPointsOnSide - 1) * 4);
    std::vector<ModelEdge> edges;
    edges.resize(howManyPointsOnSide * 2);
    float maxCoord = sideLength / 2;
    float minCoord = -maxCoord;
    float spacing = sideLength / (howManyPointsOnSide - 1);
    for (short i = 0; i < howManyPointsOnSide; i++)
    {
        points[howManyPointsOnSide * 0 + i] = { minCoord + (float)i * spacing, (float)minCoord, 0 };
        points[howManyPointsOnSide * 1 + i] = { minCoord + (float)i * spacing, (float)maxCoord, 0 };
    }
    for (size_t i = 1; i < howManyPointsOnSide - 1; i++)
    {
        points[howManyPointsOnSide * 2 - 1 + i] = { minCoord, minCoord + (float)i * spacing, 0 };
        points[howManyPointsOnSide * 3 - 3 + i] = { maxCoord, minCoord + (float)i * spacing, 0 };
    }
    for (short i = 0; i < howManyPointsOnSide; i++)
    {
        edges[i] = { (short)i, short(howManyPointsOnSide + i), axisColors[Axis::Z] };
    }
    for (short i = 0; i < howManyPointsOnSide - 2; i++)
    {
        edges[howManyPointsOnSide + i] = { (short)(howManyPointsOnSide * 2 + i), (short)(howManyPointsOnSide * 3 - 2 + i), axisColors[Axis::Z] };
    }
    edges[howManyPointsOnSide * 2 - 2] = { 0, (short)(howManyPointsOnSide - 1), axisColors[Axis::Z] };
    edges[howManyPointsOnSide * 2 - 1] = { (short)howManyPointsOnSide, (short)(howManyPointsOnSide * 2 - 1), axisColors[Axis::Z] };
    return ImGuiWireModel{ points, edges };
}
ImGuiWireModel& GetArrowModel()
{
    static ImGuiWireModel arrowModel = {
        // Points.
        {
            // Arrow origin
            {0, 0, 0},
            // Arrow tip
            {0, 0, 1},
            // Tails on tip
            {.1f, 0, .8f},
            {-.1f, 0, .8f},
            {0, 0.1f, .8f},
            {0, -0.1f, .8f},
        },
        // Edges
        {
            {0, 1, ImGui3D::axisColors[ImGui3D::Axis::Z]},
            {1, 2, ImGui3D::axisColors[ImGui3D::Axis::X]},
            {1, 3, ImGui3D::axisColors[ImGui3D::Axis::X]},
            {1, 4, ImGui3D::axisColors[ImGui3D::Axis::Y]},
            {1, 5, ImGui3D::axisColors[ImGui3D::Axis::Y]},
        } };
    return arrowModel;
}
ImGuiWireModel& GetCrossModel()
{
    static ImGuiWireModel crossModel = {
        // Points.
        {
            {-1, 0, 0},
            {1, 0, 0},
            {0, -1, 0},
            {0, 1, 0},
            {0, 0, -1},
            {0, 0, 1},
        },
        // Edges
        {
            {0, 1, ImGui3D::axisColors[ImGui3D::Axis::X]},
            {2, 3, ImGui3D::axisColors[ImGui3D::Axis::Y]},
            {4, 5, ImGui3D::axisColors[ImGui3D::Axis::Z]},
        } };
    return crossModel;
}
ImGuiWireModel& GetModel_CubePlusMinus1()
{
    static ImGuiWireModel boxModel = {
        // Points.
        {
            {-1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f, 1.0f},
            {-1.0f, 1.0f, -1.0f},
            {-1.0f, 1.0f, 1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, 1.0f},
            {1.0f, 1.0f, -1.0f},
            {1.0f, 1.0f, 1.0f},
        },
        // Edges
        {
            // Z lines
            {0, 1, IM_COL32(255, 0, 0, 255)},
            {2, 3, IM_COL32(255, 0, 0, 255)},
            {4, 5, IM_COL32(255, 0, 0, 255)},
            {6, 7, IM_COL32(255, 0, 0, 255)},
            // Top of box
            {1, 3, IM_COL32(255, 0, 0, 255)},
            {5, 7, IM_COL32(255, 0, 0, 255)},
            {1, 5, IM_COL32(255, 0, 0, 255)},
            {3, 7, IM_COL32(255, 0, 0, 255)},
            // Bottom of box
            {0, 2, IM_COL32(255, 0, 0, 255)},
            {4, 6, IM_COL32(255, 0, 0, 255)},
            {0, 4, IM_COL32(255, 0, 0, 255)},
            {2, 6, IM_COL32(255, 0, 0, 255)},
        } };
    return boxModel;
}
ImGuiWireModel& GetModel_Cube01()
{
    static ImGuiWireModel boxModel = {
        // Points.
        {
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
        },
        // Edges
        {
            // Z lines
            {0, 1, IM_COL32(255, 0, 0, 255)},
            {2, 3, IM_COL32(255, 0, 0, 255)},
            {4, 5, IM_COL32(255, 0, 0, 255)},
            {6, 7, IM_COL32(255, 0, 0, 255)},
            // Top of box
            {1, 3, IM_COL32(255, 0, 0, 255)},
            {5, 7, IM_COL32(255, 0, 0, 255)},
            {1, 5, IM_COL32(255, 0, 0, 255)},
            {3, 7, IM_COL32(255, 0, 0, 255)},
            // Bottom of box
            {0, 2, IM_COL32(255, 0, 0, 255)},
            {4, 6, IM_COL32(255, 0, 0, 255)},
            {0, 4, IM_COL32(255, 0, 0, 255)},
            {2, 6, IM_COL32(255, 0, 0, 255)},
        } };
    return boxModel;
}

struct MarkerParams
{
    Vector3f m_Location;
    float m_ScaleMultiplier = 1.0f;
};
struct LineParams
{
    Vector3f m_From;
    Vector3f m_To;
    float m_ThicknessMultiplier = 1.0f;
    ImU32 m_Color;
};
struct WireModelParams
{
    Matrix4f m_Transform;
    const ImGuiWireModel& m_Model;
};
std::vector<MarkerParams> m_LocationsOnce;
std::vector<LineParams> m_LinesOnce;
std::vector<WireModelParams> m_ModelsOnce;
struct PersistentMarker_Location
{
    std::optional<std::string> m_Name;
    MarkerParams m_Params;
};
std::atomic<int> m_IntegralIDCounter{};
using MarkerID_t = int;
std::unordered_map<std::string, MarkerID_t> m_MarkerIDforName;
std::map<MarkerID_t, PersistentMarker_Location> m_LocationsWithID;
void DrawLocationOnce(const Vector3f& location, float scaleMultiplier)
{
    m_LocationsOnce.push_back(MarkerParams{ location, scaleMultiplier });
}
void DrawLineOnce(const Vector3f& from_, const Vector3f& to_, float thicknessMultiplier, ImU32 color)
{
    m_LinesOnce.push_back(LineParams{ from_, to_, thicknessMultiplier, color });
}
void DrawWireModelOnce(const ImGui3D::ImGuiWireModel& model, const Matrix4f& transform)
{
    m_ModelsOnce.push_back(WireModelParams{ transform, model });
}
void DrawLocationAndPersist(const Vector3f& location)
{
    m_LocationsWithID[m_IntegralIDCounter++].m_Params.m_Location = location;
}
void DrawLocationNamed(const Vector3f& location, const std::string& name, float scale)
{
    auto foundID = m_MarkerIDforName.find(name);
    const bool needsNewID = foundID == m_MarkerIDforName.end();
    MarkerID_t id = needsNewID ? MarkerID_t{ m_IntegralIDCounter++ } : foundID->second;
    auto& marker = m_LocationsWithID[id];
    if (needsNewID)
    {
        m_MarkerIDforName[name] = id;
        marker.m_Name = name;
    }
    marker.m_Params.m_Location = location;
    marker.m_Params.m_ScaleMultiplier = scale;
}
std::optional<MarkerID_t> g_HoveredEditableMarkerID;
namespace CustomDraw
{
std::vector<CustomDrawer*>& GetCustomDrawers()
{
    static std::vector<CustomDrawer*> singleton;
    return singleton;
}
void CustomDraw_Subscribe(CustomDrawer& subscriber)
{
    auto& drawers = GetCustomDrawers();
    if (std::find(drawers.begin(), drawers.end(), &subscriber) != drawers.end())
    {
        return;
    }
    drawers.push_back(&subscriber);
}
void CustomDraw_Unsubscribe(CustomDrawer& subscriber)
{
    auto& drawers = GetCustomDrawers();
    drawers.erase(std::remove(drawers.begin(), drawers.end(), &subscriber), drawers.end());
}
void DrawAllCustom()
{
    for (CustomDrawer* drawer : GetCustomDrawers())
    {
        drawer->DoDraw();
    }
}
}
void DrawMarkers()
{
    for (auto& pt : m_LocationsOnce)
    {
        ImGui3D::DrawWireModel(ImGui3D::GetCrossModel(), pt.m_Location, 1.0f, pt.m_ScaleMultiplier);
    }
    m_LocationsOnce.clear();
    for (auto& ln : m_LinesOnce)
    {
        ImGuiWireModel singleLineModel{ {Vector3f(), Vector3f()}, {ModelEdge{0, 1}} };
        singleLineModel.points[0] = ln.m_From;
        singleLineModel.points[1] = ln.m_To;
        singleLineModel.edges[0].color = ln.m_Color;
        ImGui3D::DrawWireModel(singleLineModel, Vector3f(), ln.m_ThicknessMultiplier, 1.0f);
    }
    m_LinesOnce.clear();
    for (auto& [id, pt] : m_LocationsWithID)
    {
        float thicknessMultiplier = (g_HoveredEditableMarkerID && *g_HoveredEditableMarkerID == id) ? 3 : 1;
        ImGui3D::DrawWireModel(ImGui3D::GetCrossModel(), pt.m_Params.m_Location, thicknessMultiplier, pt.m_Params.m_ScaleMultiplier);
    }
    for (auto& mdl : m_ModelsOnce)
    {
        ImGui3D::DrawWireModelTransform(mdl.m_Model, mdl.m_Transform, 1.0f);
    }
    m_ModelsOnce.clear();
}

void OptionToDrawMultipleVector3fFromClipboard();
void DrawPersistent3DMarkersControls()
{
    ImGuiCTX::WindowChild _{ "PersistentMarkers", {0, 250}, true };
    if (ImGui::Button("Add"))
    {
        DrawLocationAndPersist(Vector3f());
    }
    ImGui::SameLine();
    if (ImGui::Button("Add from clipboard"))
    {
        std::optional<Vector3f> parsedVec = ParseVector3fFromClipboard();
        if (parsedVec)
        {
            DrawLocationAndPersist(*parsedVec);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear all"))
    {
        m_LocationsWithID.clear();
    }
    ImGui::SliderFloat("g_WireModelGlobalSizeMultiplier", &g_WireModelGlobalSizeMultiplier, 0.01f, 3.0f);
    OptionToDrawMultipleVector3fFromClipboard();

    static std::optional<MarkerID_t> lastSelectedMarkerIDforPopup;
    std::optional<MarkerID_t> selectedMarkerID;
    std::optional<MarkerID_t> toDelete;
    ImGui::Columns(2);
    g_HoveredEditableMarkerID.reset();
    for (auto& [id, pt] : m_LocationsWithID)
    {
        ImGui::PushID((const void*)&pt);
        ImGuiTextBuffer buf;
        buf.appendf("[%8.2f,%8.2f,%8.2f]"
            , pt.m_Params.m_Location.x
            , pt.m_Params.m_Location.y
            , pt.m_Params.m_Location.z
        );
        if (pt.m_Name)
        {
            buf.appendf(" - \"%s\""
                , pt.m_Name->c_str()
            );
        }
        if (ImGui::Selectable(buf.c_str()))
        {
            selectedMarkerID = id;
        }
        if (ImGui::IsItemHovered())
        {
            g_HoveredEditableMarkerID = id;
        }
        ImGui::NextColumn();
        if (ImGui::Button("Delete"))
        {
            toDelete = id;
        }
        if (ImGui::IsItemHovered())
        {
            g_HoveredEditableMarkerID = id;
        }
        ImGui::NextColumn();
        ImGui::PopID();
    }

    if (selectedMarkerID)
    {
        lastSelectedMarkerIDforPopup = selectedMarkerID;
        ImGui::OpenPopup("PersistentMarkerCtxMenu");
    }
    if (ImGuiCTX::Popup _{ "PersistentMarkerCtxMenu" })
    {
        if (ImGui::Button("Parse Location from Clipboard"))
        {
            std::optional<Vector3f> parsedVec = ParseVector3fFromClipboard();
            if (parsedVec)
            {
                m_LocationsWithID[lastSelectedMarkerIDforPopup.value()].m_Params.m_Location = *parsedVec;
            }
        }
        ImGui::DragFloat3("Location", m_LocationsWithID[lastSelectedMarkerIDforPopup.value()].m_Params.m_Location, 0.1f);
        if (ImGui::Button("Delete"))
        {
            toDelete = lastSelectedMarkerIDforPopup;
            ImGui::CloseCurrentPopup();
        }
    }
    if (toDelete)
    {
        m_LocationsWithID.erase(*toDelete);
    }
}

} // namespace ImGui3D


std::optional<Vector3f> ParseVector3f(const std::string_view& sv)
{
    std::stringstream ss;
    ss << sv;
    std::istream_iterator<float> the_end;
    std::istream_iterator<float> inputFloatIterator{ ss };
    Vector3f result;
    for (size_t i = 0; i < 3; i++)
    {
        if (inputFloatIterator == the_end)
        {
            // Failed to parse 3 floats.
            return {};
        }
        ((float*)&result)[i] = *inputFloatIterator;
        inputFloatIterator++;
    }
    // Parsed all 3 floats.
    return result;
}
std::optional<Vector3f> ParseVector3fFromClipboard()
{
    return ParseVector3f(ImGui::GetClipboardText());
}
#include "ACU/ACUGetSingletons.h"
#include "ACU/Entity.h"
void ImGui3D::OptionToDrawMultipleVector3fFromClipboard()
{
    static bool filterCloseToPlayer = true;
    static float filterRadiusSqr = 25;
    static Vector3f filterCloseTo;
    ImGui::Checkbox("Filter close to player", &filterCloseToPlayer);
    ImGui::SliderFloat("Filter radiusSquare", &filterRadiusSqr, 1, 100);
    if (!ImGui::Button("Visualize multiple locations from clipboard (separated by newlines)"))
    {
        return;
    }
    if (filterCloseToPlayer)
    {
        Entity* player = ACU::GetPlayer();
        if (player)
        {
            filterCloseTo = player->GetPosition();
        }
    }
    std::stringstream clipboardAllLines{ ImGui::GetClipboardText() };

    std::string line;
    while (std::getline(clipboardAllLines, line))
    {
        std::optional<Vector3f> parsedVec = ParseVector3f(line);
        if (parsedVec)
        {
            if (filterCloseToPlayer)
            {
                if ((*parsedVec - filterCloseTo).lengthSq() > filterRadiusSqr) { continue; }
            }
            DrawLocationAndPersist(*parsedVec);
        }
    }
}

#include "pch.h"

#include "ImGui3D.h"
#include "ImGuiCTX.h"

namespace ImGui3D {
Vector2f g_WindowSize;

Matrix4f g_ViewProjection;
ImDrawList* g_DrawList = nullptr;


// Point is visible <=> z-coordinate >= 0 after world-view-projection is applied;
std::optional<Vector2f> World2Screen(const Vector3f& ptWorld)
{
    Vector4f ptTransformed = g_ViewProjection * Vector4f{ ptWorld, 1 };
    if (ptTransformed.z < 0) { return {}; }
    Vector2f ptScreenSpace = ptTransformed.xy() / ptTransformed.w;
    ptScreenSpace.y *= -1;
    ptScreenSpace += {1, 1};
    ptScreenSpace *= g_WindowSize / 2;
    return ptScreenSpace;
};
const float g_WireModelDefaultThickness = 2;
void DrawWireModel(const ImGuiWireModel& model, const Vector3f& position, float thicknessMultiplier)
{
    for (size_t i = 0; i < model.points.size(); i++)
    {
        model.worldPoints[i] = position + model.points[i];
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
        edges[i] = { (short)i, (short)howManyPointsOnSide + i, axisColors[Axis::Z] };
    }
    for (short i = 0; i < howManyPointsOnSide - 2; i++)
    {
        edges[howManyPointsOnSide + i] = { (short)howManyPointsOnSide * 2 + i, (short)howManyPointsOnSide * 3 - 2 + i, axisColors[Axis::Z] };
    }
    edges[howManyPointsOnSide * 2 - 2] = { 0, (short)howManyPointsOnSide - 1, axisColors[Axis::Z] };
    edges[howManyPointsOnSide * 2 - 1] = { (short)howManyPointsOnSide, (short)howManyPointsOnSide * 2 - 1, axisColors[Axis::Z] };
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

std::vector<Vector3f> m_LocationsOnce;
struct PersistentMarker_Location
{
    Vector3f m_Location;
    std::optional<std::string> m_Name;
};
std::atomic<int> m_IntegralIDCounter{};
using MarkerID_t = int;
std::unordered_map<std::string, MarkerID_t> m_MarkerIDforName;
std::map<MarkerID_t, PersistentMarker_Location> m_LocationsWithID;
void DrawLocationOnce(const Vector3f& location)
{
    m_LocationsOnce.push_back(location);
}
void DrawLocationAndPersist(const Vector3f& location)
{
    m_LocationsWithID[m_IntegralIDCounter++].m_Location = location;
}
void DrawLocationNamed(const Vector3f& location, const std::string& name)
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
    marker.m_Location = location;
}
std::optional<MarkerID_t> g_HoveredEditableMarkerID;
void DrawMarkers()
{
    for (auto& pt : m_LocationsOnce)
    {
        ImGui3D::DrawWireModel(ImGui3D::GetCrossModel(), pt);
    }
    m_LocationsOnce.clear();
    for (auto& [id, pt] : m_LocationsWithID)
    {
        float thicknessMultiplier = (g_HoveredEditableMarkerID && *g_HoveredEditableMarkerID == id) ? 3 : 1;
        ImGui3D::DrawWireModel(ImGui3D::GetCrossModel(), pt.m_Location, thicknessMultiplier);
    }
}

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

    static std::optional<MarkerID_t> lastSelectedMarkerIDforPopup;
    std::optional<MarkerID_t> selectedMarkerID;
    std::optional<MarkerID_t> toDelete;
    ImGui::Columns(2);
    g_HoveredEditableMarkerID.reset();
    for (auto& [id, pt] : m_LocationsWithID)
    {
        ImGui::PushID((const void*)&pt);
        std::string asString = pt.m_Location.toString();
        if (pt.m_Name)
        {
            asString.append(" - \"" + *pt.m_Name + '"');
        }
        if (ImGui::Selectable(asString.c_str()))
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
                m_LocationsWithID[lastSelectedMarkerIDforPopup.value()].m_Location = *parsedVec;
            }
        }
        ImGui::DragFloat3("Location", m_LocationsWithID[lastSelectedMarkerIDforPopup.value()].m_Location, 0.1f);
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


std::optional<Vector3f> ParseVector3fFromClipboard()
{
    std::stringstream ss;
    ss << ImGui::GetClipboardText();
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

#include "pch.h"

#include "base.h"
#include "vmath/vmath.h"
#include "ImGuizmo/ImGuizmo.h"
#include "ImGui3D.h"
#include "ImGuiCTX.h"



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
#define IMGUI_DUMPHEX(x)\
{\
    std::stringstream ss;\
    ss << #x << ": " << std::hex << (uintptr_t)x;\
    ImGui::Text(ss.str().c_str());\
}
Vector3f g_VisualizedDebugLocation;
Vector3f g_VisualizedDebugDirection;
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
void VisualizeLocationFromClipboard()
{
    g_VisualizedDebugLocation = ParseVector3fFromClipboard().value_or(g_VisualizedDebugLocation);
}
void VisualizeDirectionFromClipboard()
{
    g_VisualizedDebugDirection = ParseVector3fFromClipboard().value_or(g_VisualizedDebugDirection);
}
void Base::ImGuiLayer_WhenMenuIsOpen()
{
    ImGui::GetIO().MouseDrawCursor = true;
    if (ImGui::Begin("Stuff"))
    {
        if (ImGui::Button("Visualize location from clipboard"))
        {
            VisualizeLocationFromClipboard();
        }
        if (ImGui::Button("Visualize direction from clipboard"))
        {
            VisualizeDirectionFromClipboard();
        }
        IMGUI_DUMPHEX(Data::pSwapChain);
        IMGUI_DUMPHEX(Data::pPresent);
        IMGUI_DUMPHEX(Data::pDxDevice11);
        IMGUI_DUMPHEX(Data::pContext);
        ImGui::Text("Projection tuning:");
        ImGui::Text("View:");
        ImGuiPrintMatrix(gameMatView);
        ImGui::Text("Proj:");
        ImGuiPrintMatrix(gameMatProj);
    }
    ImGui::End();
}

#include "ACUGetSingletons.h"
#include "Entity.h"
#include "RenderValuesHolder.h"

Matrix4f MakeSimpleDebugTransform(const Vector3f& position)
{
    return Matrix4f::createTranslation(position.x, position.y, position.z) * Matrix4f::createScale(0.1f, 0.1f, 0.1f);
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


struct ChooseLessAlignedVector
{
    Vector3f moreAligned;
    Vector3f lessAligned;
    ChooseLessAlignedVector(const Vector3f& alignedWith, const Vector3f& _1, const Vector3f _2)
    {
        float al1 = abs(alignedWith.dotProduct(_1));
        float al2 = abs(alignedWith.dotProduct(_2));
        moreAligned = (al1 < al2) ? _2 : _1;
        lessAligned = (al1 < al2) ? _1 : _2;
    }
};
Matrix3f MakeRotationAlignZWithVector(Vector3f axisZ)
{
    axisZ.normalize();
    const Vector3f regularX = { 1, 0, 0 };
    const Vector3f regularY = { 0, 1, 0 };
    ChooseLessAlignedVector otherAxes{ axisZ, regularX, regularY };
    Vector3f axisX = otherAxes.lessAligned;
    Vector3f axisY = axisX.crossProduct(axisZ).normalized();
    axisX.normalize();
    float result[9] = {
        axisX.x, axisX.y, axisX.z,
        axisY.x, axisY.y, axisY.z,
        axisZ.x, axisZ.y, axisZ.z };
    return result;
}

void ImGuizmoLayer()
{
    SetProjMatrix(gameMatProj);
    SetCorrectViewMatrix(gameMatView);

    // On a pile of junk next to the artiste.
    static Vector3f testPosition{ 127.82f, 704.28f, 1.06f };
    static Matrix4f transformGrid = MakeSimpleDebugTransform(testPosition) * Matrix4f::createRotationAroundAxis(-90, 0, 0);

    static std::vector<Matrix4f> cubeTransforms = []() {std::vector<Matrix4f> vec; vec.reserve(16); return vec; }();
    cubeTransforms.clear();
    cubeTransforms.push_back(MakeSimpleDebugTransform(testPosition));
    Entity* player = ACU::GetPlayer();
    if (player)
    {
        cubeTransforms.push_back(MakeSimpleDebugTransform(player->GetPosition()));
    }
    ImGuizmo::DrawCubes((float*)&gameMatView, (float*)&gameMatProj, (float*)cubeTransforms.data(), (int)cubeTransforms.size());

    static ImGui3D::ImGuiWireModel grid5_model = ImGui3D::GenerateGrid(5, 2);

    ImGui3D::g_ViewProjection = gameMatProj * gameMatView;
    ImGui3D::g_DrawList = ImGui::GetWindowDrawList();

    Matrix4f debugDirectionTransform;
    debugDirectionTransform.setRotation(MakeRotationAlignZWithVector(g_VisualizedDebugDirection));
    debugDirectionTransform = Matrix4f::createTranslation(player->GetPosition()) * debugDirectionTransform;
    ImGui3D::DrawWireModelTransform(ImGui3D::GetArrowModel(), debugDirectionTransform);
    ImGui3D::DrawWireModel(ImGui3D::GetCrossModel(), g_VisualizedDebugLocation);
    ImGui3D::DrawWireModel(ImGui3D::GetArrowModel(), testPosition);
    ImGui3D::DrawWireModelTransform(ImGui3D::GetArrowModel(), player->GetTransform());
    ImGui3D::DrawWireModelTransform(grid5_model, player->GetTransform());
}
void DrawImGuizmo()
{
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();
    // ImGuizmo only draws within the bounds of an ImGui Window.
    // Create a window the size of the screen with a transparent background.
    ImVec2 windowSize = { 1680, 1050 };
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
    ImGuiWindowFlags imguizmoWindowFlags = 0;
    imguizmoWindowFlags |= ImGuiWindowFlags_NoBackground;
    imguizmoWindowFlags |= ImGuiWindowFlags_NoDecoration;
    imguizmoWindowFlags |= ImGuiWindowFlags_NoMove;
    imguizmoWindowFlags |= ImGuiWindowFlags_NoResize;
    imguizmoWindowFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
    imguizmoWindowFlags |= ImGuiWindowFlags_NoNav;
    imguizmoWindowFlags |= ImGuiWindowFlags_NoInputs;

    if (ImGui::Begin( "ImGuizmos", 0, imguizmoWindowFlags))
    {
        // More setup.
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(0, 0, windowSize.x, windowSize.y);
        ImGuizmo::Enable(false);
        // Draw ImGuizmo's gizmos, cubes, grids, etc.
        ImGuizmoLayer();
    }
    ImGui::End();
}
void Base::ImGuiLayer_EvenWhenMenuIsClosed()
{
    DrawImGuizmo();

    ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    window_flags |= ImGuiWindowFlags_NoNav;
    window_flags |= ImGuiWindowFlags_NoInputs;

    if (ImGui::Begin("Always enabled overlay", nullptr, window_flags ))
    {
        ImGui::Text("Overlay on. Press INSERT to open ImGui menu.");
    }
    ImGui::End();
}

#include "ConsoleForOutput.h"

void DisableMainIntegrityCheck();
DWORD WINAPI MainThread(LPVOID lpThreadParameter)
{
    {
    ConsoleForOutput _console;
    RedirectSTDOUTToConsole _stdout;
    std::cout << "Opened console." << std::endl;
    DisableMainIntegrityCheck();
	Base::Data::hModule = (HMODULE)lpThreadParameter;
	Base::Init();
    Base::Data::ShowMenu = false;
    while (!Base::Data::Detached)
    {
        Sleep(100);
    }
    }
    FreeLibraryAndExitThread(Base::Data::hModule, TRUE);
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	default:
		break;
	}
	return TRUE;
}
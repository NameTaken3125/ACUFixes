#include "pch.h"
#include "base.h"
#include "ImGuizmo/ImGuizmo.h"


#include "vmath/vmath.h"


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
void Base::ImGuiLayer_WhenMenuIsOpen()
{
    if (ImGui::Begin("Stuff"))
    {
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


Matrix4f MakeMatProj()
{
    float m[16]{
        1.399690509f, 0, 0, 0,
        0, 2.488338709f, 0.001058201073f, 0,
        0, 0, 0, 0.1000022292f,
        0, 0, -1, 0 };
    return Matrix4f{ m }.transpose();
}
Matrix4f MakeSimpleDebugTransform(const Vector3f& position)
{
    return Matrix4f::createTranslation(position.x, position.y, position.z) * Matrix4f::createScale(0.1f, 0.1f, 0.1f);
}
void GetProj(Matrix4f& matOut)
{
    static Matrix4f gameMatProj = MakeMatProj();
    matOut = gameMatProj;
}
void SetProjMatrix()
{
    GetProj(gameMatProj);
}
// The stupid game has the camera matrix all rotated around.
// Why don't you stop rotating your matrices, game?
void SetCorrectViewMatrix(Matrix4f& matOut)
{
    Matrix4f cameraMat = **(Matrix4f**)0x14521AAA8;
    cameraMat = cameraMat * Matrix4f::createRotationAroundAxis(-90, 0, 0);
    matOut = cameraMat.inverse();
}

void ImGuizmoLayer()
{
    SetProjMatrix();
    SetCorrectViewMatrix(gameMatView);

    // On a pile of junk next to the artiste.
    static Vector3f pos{ 127.82f, 704.28f, 1.06f };
    static Matrix4f transformCube = MakeSimpleDebugTransform(pos);
    static Matrix4f transformGrid = transformCube;

    //static glm::vec3 pos{ 127.82, 704.28, 1.06 };
    //static glm::mat4 transformCube = MakeSimpleDebugTransform(pos);
    //static glm::mat4 transformGrid = transformCube;

    ImGuizmo::DrawGrid((float*)&gameMatView, (float*)&gameMatProj, (float*)&transformGrid, 5);
    ImGuizmo::DrawCubes((float*)&gameMatView, (float*)&gameMatProj, (float*)&transformCube, 1);
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

    ImGui::SetNextWindowBgAlpha(0.1f); // Transparent background
    ImVec2 overlayWindowSize{ 200, 33 };
    ImGui::SetNextWindowSize(overlayWindowSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    window_flags |= ImGuiWindowFlags_NoNav;
    window_flags |= ImGuiWindowFlags_NoInputs;

    if (ImGui::Begin("Always enabled overlay", nullptr, window_flags ))
    {
        ImGui::Text("Overlay on");
    }
    ImGui::End();
}

DWORD WINAPI MainThread(LPVOID lpThreadParameter)
{
	Base::Data::hModule = (HMODULE)lpThreadParameter;
	Base::Init();
    while (!Base::Data::Detached)
    {
        Sleep(100);
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
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
void Base::ImGuiLayer_WhenMenuIsOpen()
{
    static bool enableDemoWindow = false;
    if (enableDemoWindow) {
        ImGui::ShowDemoWindow();
    }
    if (ImGuiCTX::Window _mainWindow{ "Stuff" })
    {
        if (ImGuiCTX::TabBar _tabbar{ "MainWindowTabs" })
        {
            if (ImGuiCTX::Tab _mainTab{ "Main Tab" })
            {
                DrawHacksControls();
                if (ImGui::CollapsingHeader("View-projection matrices debugging"))
                {
                    ImGui::Text("Projection tuning:");
                    ImGui::Text("View:");
                    ImGuiPrintMatrix(gameMatView);
                    ImGui::Text("Proj:");
                    ImGuiPrintMatrix(gameMatProj);
                }
            }
            if (ImGuiCTX::Tab _3dMarkersTab{ "3D Markers" })
            {
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
            }
            if (ImGuiCTX::Tab _typeInfosTab{ "TypeInfos" })
            {
                TypeInfoSystemTests();
            }
            if (ImGuiCTX::Tab _typeInfosTab{ "DX11-BaseHook veriables" })
            {
                Base::ImGuiDrawBasehookDebug();
            }
            if (ImGuiCTX::Tab _typeInfosTab{ "ImGui demo" })
            {
                ImGui::Checkbox("Show ImGui demo window", &enableDemoWindow);
            }
        }
    }
}

Matrix4f MakeSimpleDebugTransform(const Vector3f& position)
{
    return Matrix4f::createTranslation(position.x, position.y, position.z) * Matrix4f::createScale(0.1f, 0.1f, 0.1f);
}



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
        ImGui::Text("Overlay on. Press INSERT to open ImGui menu.");
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
void ImGui3D::WhatIsActuallyDrawnForFrame()
{
    ImGui3D::DrawMarkers();
    // On a pile of junk next to the artiste.
    static Vector3f testPosition{ 127.82f, 704.28f, 1.06f };
    static Matrix4f transformGrid = MakeSimpleDebugTransform(testPosition) * Matrix4f::createRotationAroundAxis(-90, 0, 0);

    Entity* player = ACU::GetPlayer();

    static ImGui3D::ImGuiWireModel grid5_model = ImGui3D::GenerateGrid(5, 2);

    ImGui3D::DrawWireModel(ImGui3D::GetArrowModel(), testPosition);
    if (player)
    {
        Matrix4f debugDirectionTransform;
        debugDirectionTransform.setRotation(MakeRotationAlignZWithVector(g_VisualizedDebugDirection));
        debugDirectionTransform = Matrix4f::createTranslation(player->GetPosition()) * debugDirectionTransform;
        ImGui3D::DrawWireModelTransform(ImGui3D::GetArrowModel(), debugDirectionTransform);
        ImGui3D::DrawWireModelTransform(ImGui3D::GetArrowModel(), player->GetTransform());
        ImGui3D::DrawWireModelTransform(grid5_model, player->GetTransform());
    }
}
void Base::ImGuiLayer_EvenWhenMenuIsClosed()
{
    ImGui3D::DrawStuff();
    DrawSuccessfulInjectionIndicatorOverlay();
}
#include "ConsoleForOutput.h"
#include "PresentHookOuter.h"

#define PRESENT_HOOK_METHOD_INNER 1
#define PRESENT_HOOK_METHOD_OUTER 2

#define PRESENT_HOOK_METHOD PRESENT_HOOK_METHOD_OUTER
#ifndef PRESENT_HOOK_METHOD
static_assert(false, "PRESENT_HOOK_METHOD macro needs to be defined. See `main.cpp` for options.");
#endif // !PRESENT_HOOK_METHOD


void DisableMainIntegrityCheck();
static void MainThread(HMODULE thisDLLModule)
{
    ConsoleForOutput _console;
    RedirectSTDOUTToConsole _stdout;
    std::cout << "Opened console." << std::endl;
    DisableMainIntegrityCheck();
    Base::Data::thisDLLModule = thisDLLModule;
#if PRESENT_HOOK_METHOD == PRESENT_HOOK_METHOD_OUTER
    Base::Init(false);
    PresentHookOuter::Activate();
#elif PRESENT_HOOK_METHOD == PRESENT_HOOK_METHOD_INNER
    Base::Init(true);
#endif
    Base::Data::ShowMenu = false;
    while (!Base::Data::Detached)
    {
        Sleep(100);
    }
}
DWORD WINAPI MainThreadRAIIWrapper(LPVOID lpThreadParameter)
{
    // `FreeLibraryAndExitThread()` will prevent destructors of objects at this scope
    // from being called, that's why all work is instead done in a separate function.
    MainThread((HMODULE)lpThreadParameter);
    FreeLibraryAndExitThread((HMODULE)lpThreadParameter, TRUE);
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, MainThreadRAIIWrapper, hModule, 0, nullptr);
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
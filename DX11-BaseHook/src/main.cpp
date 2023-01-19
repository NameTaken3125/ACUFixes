#include "pch.h"
#include "base.h"

void Base::ImGuiLayer_WhenMenuIsOpen()
{
    if (ImGui::Begin("Debug"))
    {
#define IMGUI_DUMPHEX(x)\
{\
    std::stringstream ss;\
    ss << #x << ' ' << std::hex << (uintptr_t)x;\
    ImGui::Text(ss.str().c_str());\
}
        IMGUI_DUMPHEX(Data::pSwapChain);
        IMGUI_DUMPHEX(Data::pPresent);
        IMGUI_DUMPHEX(Data::pDxDevice11);
        IMGUI_DUMPHEX(Data::pContext);
    }
    ImGui::End();
}
void Base::ImGuiLayer_EvenWhenMenuIsClosed()
{
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
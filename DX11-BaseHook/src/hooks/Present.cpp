#include <pch.h>
#include <base.h>


namespace Base {
namespace Hooks {
void GrabGraphicsDevicesInitializeImGui(IDXGISwapChain* swapchain);
}
}
HRESULT PRESENT_CALL Base::Hooks::Present(IDXGISwapChain* thisptr, UINT SyncInterval, UINT Flags)
{
    GrabGraphicsDevicesInitializeImGuiAndDraw(thisptr);
    return Base::Data::oPresent(thisptr, SyncInterval, Flags);
}
void Base::Hooks::GrabGraphicsDevicesInitializeImGui(IDXGISwapChain* thisptr)
{
    Data::pSwapChain = thisptr;
    if (SUCCEEDED(Data::pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Data::pDxDevice11)))
    {
        Data::pDxDevice11->GetImmediateContext(&Data::pContext);
        DXGI_SWAP_CHAIN_DESC sd;
        Data::pSwapChain->GetDesc(&sd);
        Data::hWindow = sd.OutputWindow;
        ID3D11Texture2D* pBackBuffer;
        Data::pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        Data::pDxDevice11->CreateRenderTargetView(pBackBuffer, NULL, &Data::pMainRenderTargetView);
        pBackBuffer->Release();
        Data::oWndProc = (WNDPROC)SetWindowLongPtr(Data::hWindow, GWLP_WNDPROC, (LONG_PTR)g_Settings->m_WndProc);

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
        ImGui_ImplWin32_Init(Data::hWindow);
        ImGui_ImplDX11_Init(Data::pDxDevice11, Data::pContext);
        Data::IsImGuiInitialized = true;
    }
}
void Base::Hooks::GrabGraphicsDevicesInitializeImGuiAndDraw(IDXGISwapChain* thisptr)
{
	if (!Data::IsImGuiInitialized)
	{
        GrabGraphicsDevicesInitializeImGui(thisptr);
	}

    if (!Data::IsImGuiInitialized) return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
    Base::ImGuiLayer_EvenWhenMenuIsClosed();
	if (Data::ShowMenu)
	{
        Base::ImGuiLayer_WhenMenuIsOpen();
        //ImGui::Begin("ImGui Window");
        //ImGui::Text("Test ImGUI Window");
        //if (ImGui::Button("Detach"))
        //{
        //    ImGui::End();
        //    ImGui::EndFrame();
        //    ImGui::Render();
        //    Data::pContext->OMSetRenderTargets(1, &Data::pMainRenderTargetView, NULL);
        //    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        //    Base::Detach();
        //    return;
        //}
        //ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();

	Data::pContext->OMSetRenderTargets(1, &Data::pMainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return;
}

void DrawVariableDebug(const char* varName, uintptr_t integerVar)
{
    ImGui::Text("%s: 0x%llx", varName, integerVar);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Click to copy address to clipboard");
    if (ImGui::IsItemClicked(0)) {
        ImGui::LogToClipboard();
        ImGui::LogText("0x%llx", integerVar);
        ImGui::LogFinish();
    }
}
#define TOSTRINGMACRO(var) #var
#define DUMPHEX(integerVar)\
    DrawVariableDebug(#integerVar, (uintptr_t)integerVar);
#define DUMPBOOL(boolVar)\
    ImGui::Text(boolVar ? #boolVar ": true" : #boolVar ": false");
void Base::ImGuiDrawBasehookDebug()
{
    ImGui::BeginChild("basehookVariables", ImVec2(0, ImGui::GetFontSize() * 20.0f), true);
    DUMPHEX(Base::Data::thisDLLModule);
    DUMPHEX(Base::Data::hWindow);
    DUMPHEX(Base::Data::pSwapChain);
    DUMPHEX(Base::Data::pPresent);
    DUMPHEX(Base::Data::pDxDevice11);
    DUMPHEX(Base::Data::pContext);
    DUMPHEX(Base::Data::oWndProc);
    DUMPBOOL(Base::Data::IsImGuiInitialized);
    DUMPBOOL(Base::Data::IsUsingPresentInnerHook);
    ImGui::EndChild();
}

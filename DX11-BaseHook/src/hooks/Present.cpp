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
        Data::oWndProc = (WNDPROC)SetWindowLongPtr(Data::hWindow, GWLP_WNDPROC, (LONG_PTR)Hooks::WndProc);

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
		ImGui::Begin("ImGui Window");
		ImGui::Text("Test ImGUI Window");
		if (ImGui::Button("Detach"))
		{
			ImGui::End();
			ImGui::EndFrame();
			ImGui::Render();
			Data::pContext->OMSetRenderTargets(1, &Data::pMainRenderTargetView, NULL);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			Base::Detach();
            return;
		}
		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();

	Data::pContext->OMSetRenderTargets(1, &Data::pMainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return;
}
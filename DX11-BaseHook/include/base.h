#pragma once
#ifndef BASE_H
#define BASE_H

#pragma comment(lib, "d3d11.lib")

#include "libmem++/old-libmem-platform-defines.h"
#include <Windows.h>
struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;

#if defined(MEM_86)
#define WNDPROC_INDEX GWL_WNDPROC
#define PRESENT_CALL __stdcall
#elif defined(MEM_64)
#define WNDPROC_INDEX GWLP_WNDPROC
#define PRESENT_CALL __fastcall
#endif
#define D3D11DEV_LEN 40
#define D3D11SC_LEN  18
#define D3D11CTX_LEN 108

typedef HRESULT(PRESENT_CALL* Present_t)(IDXGISwapChain*, UINT, UINT);
typedef LRESULT(CALLBACK*  WndProc_t) (HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI MainThread(LPVOID lpThreadParameter);
DWORD WINAPI ExitThread(LPVOID lpThreadParameter);

namespace Base
{
class Settings
{
public:
    virtual void OnBeforeDetach() = 0;
    virtual WNDPROC GetWNDPROC() = 0;
    virtual void OnBeforeActivate() = 0;
};
}
namespace Base
{
	bool Init(bool usePresentInnerHook);
	bool Shutdown();
	bool Detach();

    // To be implemented by user.
    void ImGuiLayer_WhenMenuIsOpen();
    void ImGuiLayer_EvenWhenMenuIsClosed();
    extern Settings* g_Settings;
    // Information like Swapchain address, original WndProc address
    // helps to transition to outer hooks.
    void ImGuiDrawBasehookDebug();

    using voidptr_t = void*;
    using size_t = ::size_t;
	namespace Data
	{
		extern HMODULE                 thisDLLModule;
		extern ID3D11Device*           pDxDevice11;
		extern IDXGISwapChain*         pSwapChain;
		extern ID3D11DeviceContext*    pContext;
		extern ID3D11RenderTargetView* pMainRenderTargetView;
		extern void*                   pDeviceTable   [D3D11DEV_LEN];
		extern void*                   pSwapChainTable[D3D11SC_LEN];
		extern void*                   pContextTable  [D3D11CTX_LEN];
		extern HWND                    hWindow;
		extern voidptr_t               pPresent;
		extern Present_t               oPresent;
		extern WndProc_t               oWndProc;
		extern size_t                  szPresent;
		extern UINT                    WmKeys[0xFF];
		extern bool                    Detached;
		extern bool                    IsImGuiInitialized;
		extern bool                    ShowMenu;
		extern bool                    IsUsingPresentInnerHook;

		namespace Keys
		{
			const UINT ToggleMenu = VK_INSERT;
			const UINT DetachDll  = VK_END;
		}
	}

	namespace Hooks
	{
		bool Init(bool usePresentInnerHook);
		bool Shutdown();
		HRESULT PRESENT_CALL Present(IDXGISwapChain* thisptr, UINT SyncInterval, UINT Flags);
		LRESULT CALLBACK  WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        // This is called in Hooks::Present() if the "inner" hook is activated.
        // If you want to use an "outer" hook (at the call site), just call this function from there.
        void GrabGraphicsDevicesInitializeImGuiAndDraw(IDXGISwapChain* thisptr);
	}
}

#endif
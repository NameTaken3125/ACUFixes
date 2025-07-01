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
#include <filesystem>
namespace fs = std::filesystem;
void* ImFileLoadToMemory(const char* filename, const char* mode, size_t* out_file_size, int padding_bytes);
int ImFormatString(char* buf, size_t buf_size, const char* fmt, ...);
bool    ImGui_ImplDX11_CreateDeviceObjects();
#include "imgui/imgui_internal.h"
namespace Base::Fonts
{

float g_FontsSetup_TargetSize = 13.0f;
bool g_DoResetFonts = false;

void SetFontSize(float fontSizePixels)
{
    g_FontsSetup_TargetSize = fontSizePixels;
    g_DoResetFonts = true;
}



ImFont* Atlas__AddFontFromFileTTF(ImFontAtlas& atlas, const char* filename, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    IM_ASSERT(!atlas.Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    size_t data_size = 0;
    void* data = ImFileLoadToMemory(filename, "rb", &data_size, 0);
    if (!data)
    {
        return NULL;
    }
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (font_cfg.Name[0] == '\0')
    {
        // Store a short copy of filename into into the font name for convenience
        const char* p;
        for (p = filename + strlen(filename); p > filename && p[-1] != '/' && p[-1] != '\\'; p--) {}
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s, %.0fpx", p, size_pixels);
    }

    return atlas.AddFontFromMemoryTTF(data, (int)data_size, size_pixels, &font_cfg, glyph_ranges);
}
ImFont* AddDefaultFontWithSize(ImFontAtlas& atlas, float fontSize)
{
    ImFontConfig font_cfg;
    font_cfg.SizePixels = fontSize;
    font_cfg.OversampleH = font_cfg.OversampleV = 1;
    font_cfg.PixelSnapH = true;
    return atlas.AddFontDefault(&font_cfg);
}
fs::path FindPreferredFontFile()
{
    wchar_t windir[MAX_PATH];
    UINT windirLength = GetWindowsDirectoryW(windir, MAX_PATH);
    if (!windirLength) return {};
    fs::path fontsDir = fs::path(windir) / "Fonts";
    if (!fs::is_directory(fontsDir)) return {};
    fs::path preferredFont =        fontsDir / "CascadiaCode.ttf";  // Not present on Windows 8
    fs::path secondPreferredFont =  fontsDir / "consola.ttf";       // Or maybe the Bold version, the consolab.ttf?
    std::error_code ec;
    if (fs::is_regular_file(preferredFont, ec))         return preferredFont;
    if (fs::is_regular_file(secondPreferredFont, ec))   return secondPreferredFont;
    return {};
}
void SetupFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontDefault();
    ImFont* mainFont = nullptr;
    fs::path fontFilepath = FindPreferredFontFile();
    if (!fontFilepath.empty())
    {
        mainFont = Atlas__AddFontFromFileTTF(*io.Fonts,
            (const char*)fontFilepath.u8string().c_str()
            , g_FontsSetup_TargetSize
            , NULL, io.Fonts->GetGlyphRangesCyrillic()
        );
    }
    if (mainFont == nullptr)
    {
        mainFont = AddDefaultFontWithSize(*io.Fonts, g_FontsSetup_TargetSize);
    }
    io.FontDefault = mainFont;
    //io.FontAllowUserScaling = true;
    io.Fonts->Build();
}
void UpdateFonts()
{
    if (!g_DoResetFonts) return;
    g_DoResetFonts = false;
    SetupFonts();
    ImGui::GetCurrentContext()->Style = ImGuiStyle();
    ImGui::GetCurrentContext()->Style.ScaleAllSizes(g_FontsSetup_TargetSize / 13.0f);
    ImGui_ImplDX11_CreateDeviceObjects();
}
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

        Fonts::SetupFonts();
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
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    Base::Fonts::UpdateFonts();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0, 0.0, 0.0, 0));
    ImGui::DockSpaceOverViewport(0, 0, ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::PopStyleColor();
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

#include "pch.h"

#include "ImGuiTextWindows.h"

ImGui::ReadOnlyTextWindow g_memoryQueryLog{ "cememoryquerylog" };


uintptr_t g_moduleBase = 0x140000000;
#define IMGUI_DUMPHEX(x)\
{\
    ImGui::Text(#x##": %llx", x);\
}
void* FindFreeBlockForRegion(uintptr_t base, unsigned int size);
void CheatEngineMemoryQuery_TEST()
{
    static void* var = nullptr;
    static void* code = nullptr;
    if (ImGui::Button("Find free regions"))
    {
        var = FindFreeBlockForRegion(0x3500000, 8);
        code = FindFreeBlockForRegion(g_moduleBase + 0x2739C90, 0x1000);
    }
    IMGUI_DUMPHEX(var);
    IMGUI_DUMPHEX(code);
    g_memoryQueryLog.Draw();
}

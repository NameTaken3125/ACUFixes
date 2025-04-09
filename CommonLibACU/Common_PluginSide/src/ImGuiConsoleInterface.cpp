#include "pch.h"

#include "Common_Plugins_impl/ImGuiConsoleInterface.h"
#include "Common_Plugins/Console/ImGuiConsole.h"

ImGuiConsoleInterface* g_ImGuiConsoleInterfacePtr = nullptr;


namespace ImGuiConsole
{
void AddLog(const char* s)
{
    g_ImGuiConsoleInterfacePtr->fnp_AddLog(s);
}
} // namespace ImGuiConsole

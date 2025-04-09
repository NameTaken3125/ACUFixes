#include "pch.h"

#include "Common_Plugins_impl/ImGuiConsoleInterface.h"
#include "ImGuiConsole.h"

ImGuiConsoleInterface g_ImGuiConsoleInterface;

void ImGuiConsole_AddLog(const char* s)
{
    ImGuiConsole::GetSingleton().AddLog(s);
}

ImGuiConsoleInterface::ImGuiConsoleInterface()
    : fnp_AddLog(::ImGuiConsole_AddLog)
{}

#pragma once

struct ImGuiConsoleInterface
{
    void (*fnp_AddLog)(const char* s);

    ImGuiConsoleInterface(); // Defined in PluginLoader project.
};

extern ImGuiConsoleInterface* g_ImGuiConsoleInterfacePtr; // Pointer is stored in the plugin; points to the instance owned by the PluginLoader.

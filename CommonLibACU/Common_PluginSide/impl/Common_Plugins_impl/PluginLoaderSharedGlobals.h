#pragma once

#include "Common_Plugins_impl/InputHooks.h"
class AnimationModdingInterface;
struct ImGuiConsoleInterface;
class ACUPluginLoaderSharedGlobals
{
public:
    ACU::Input::InputHooks& m_InputHooks;
    AnimationModdingInterface& m_AnimationModding;
    ImGuiConsoleInterface& m_ImGuiConsole;

    ACUPluginLoaderSharedGlobals(); // Defined in PluginLoader project.
};
class ImGuiShared
{
public:
    ImGuiContext& m_ctx;
    void* (*alloc_func)(size_t sz, void* user_data);
    void (*free_func)(void* ptr, void* user_data);
    void* user_data;
};

#pragma once

#include "Common_Plugins_impl/InputHooks.h"
class ACUPluginLoaderSharedGlobals
{
public:
    ACU::Input::InputHooks& m_InputHooks;
    ACUPluginLoaderSharedGlobals(); // Defined in PluginLoader project.
};

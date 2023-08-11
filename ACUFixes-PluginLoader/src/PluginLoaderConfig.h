#pragma once

#include "Enum_VirtualKeys.h"

extern const char* g_PluginLoaderConfig_Filename;

namespace PluginLoaderConfig {

void FindAndLoadConfigFileOrCreateDefault();
void WriteToFile();

} // namespace MainConfig

#include "OLYAFSer/OLYAFSer.h"
#include "Serialization/EnumAdapter.h"

#define ACM(varName, VarType, AdapterType, optionalDefaultValue) ADD_CONFIG_MEMBER(varName, VarType, AdapterType, optionalDefaultValue)
#define YACSTOR(SubclsName) YACONFIGSECTION_SUBCLASS_CTOR(SubclsName)
struct ConfigTop : YAConfigSection {
    YACSTOR(ConfigTop);
    ACM(imgui_showSuccessfulInjectionIndicator, bool, BooleanAdapter, true);
    ACM(hotkey_ToggleMenu, VirtualKeys, EnumAdapter_template<VirtualKeys>, VirtualKeys::KEYBOARD_INSERT);
    ACM(hotkey_UnloadMod, VirtualKeys, EnumAdapter_template<VirtualKeys>, VirtualKeys::KEYBOARD_END);
};
extern ConfigTop g_PluginLoaderConfig;

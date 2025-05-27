#pragma once

#include "Enum_VirtualKeys.h"

extern const char* g_PluginLoaderConfig_Filename;

namespace PluginLoaderConfig {

void FindAndLoadConfigFileOrCreateDefault();
void WriteToFile();

} // namespace MainConfig

#include "OLYAFSer/OLYAFSer.h"
#include "Serialization/EnumAdapter.h"
#include "Serialization/NumericAdapters.h"

#define ACM(varName, VarType, AdapterType, optionalDefaultValue) ADD_CONFIG_MEMBER(varName, VarType, AdapterType, optionalDefaultValue)
#define YACSTOR(SubclsName) YACONFIGSECTION_SUBCLASS_CTOR(SubclsName)
struct ConfigTop : YAConfigSection {
    YACSTOR(ConfigTop);
    ACM(imgui_showSuccessfulInjectionIndicator, bool, BooleanAdapter, true);
    ACM(hotkey_ToggleMenu, VirtualKeys, EnumAdapter_template<VirtualKeys>, VirtualKeys::KEYBOARD_INSERT);
    ACM(hotkey_ToggleConsole, VirtualKeys, EnumAdapter_template<VirtualKeys>, VirtualKeys::KEYBOARD_OEM3);
    ACM(fontSize, float, NumericAdapter_template<float>, 16.0f);
    struct YACS_DeveloperOptions : YAConfigSection {
        YACSTOR(YACS_DeveloperOptions);
        ACM(isActive, bool, BooleanAdapter, false);
        struct YACS_UninjectPluginLoader : YAConfigSection {
            YACSTOR(YACS_UninjectPluginLoader);
            ACM(isActive, bool, BooleanAdapter, false);
            ACM(hotkey_UninjectPluginLoader, VirtualKeys, EnumAdapter_template<VirtualKeys>, VirtualKeys::KEYBOARD_END);
        };
        ACM(canUninjectPluginLoader, YACS_UninjectPluginLoader, YAConfigSectionAdapter, );
        ACM(hotkey_UnloadMod, VirtualKeys, EnumAdapter_template<VirtualKeys>, VirtualKeys::KEYBOARD_END);
        ACM(hungrierCrashLog_hookZwRaiseException, bool, BooleanAdapter, false);
    };
    ACM(developerOptions, YACS_DeveloperOptions, YAConfigSectionAdapter, );
};
extern ConfigTop g_PluginLoaderConfig;

#pragma once

namespace json { class JSON; }
using json::JSON;

#include "Enum_BindableKeyCode_Keyboard.h"
extern BindableKeyCode_Keyboard enterWindowsButton;
extern BindableKeyCode_Keyboard autowalkButton;

namespace MainConfig {

extern bool imgui_useImGui;
extern bool imgui_showSuccessfulInjectionIndicator;

void FindAndLoadConfigFileOrCreateDefault();
void WriteToFile();
JSON& GetConfigJSON();

} // namespace MainConfig

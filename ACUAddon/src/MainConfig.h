#pragma once

namespace json { class JSON; }
using json::JSON;

namespace MainConfig {

extern bool imgui_useImGui;
extern bool imgui_showSuccessfulInjectionIndicator;

void FindAndLoadConfigFileOrCreateDefault();
JSON& Get();

} // namespace MainConfig

#pragma once

namespace MainConfig {

extern bool imgui_useImGui;
extern bool imgui_showSuccessfulInjectionIndicator;

void FindAndLoadConfigFileOrCreateDefault();

} // namespace MainConfig

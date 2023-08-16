#pragma once

extern ImGuiContext* GImGui;
class ACUPluginInterfaceVirtuals
{
public:
	virtual ~ACUPluginInterfaceVirtuals() {}
	virtual void ImGui_WhenMenuIsOpen() = 0;
	virtual void ImGui_EvenWhenMenuIsClosed() = 0;

public:
    ImGuiContext*& m_imguiCtxPtr = GImGui;
};

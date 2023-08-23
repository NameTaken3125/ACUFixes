#pragma once

/*
"CTX" as in "context manager" from Python.
A number of simple scoped RAII wrappers for pairs of ImGui::Begin...() and ImGui::End...() functions.
*/
namespace ImGuiCTX
{
class Window
{
    bool m_isOpened = false;
public:
    Window(const std::string_view& windowName, bool* p_open = NULL, ImGuiWindowFlags flags = 0)
    {
        m_isOpened = ImGui::Begin(windowName.data(), p_open, flags);
    }
    ~Window()
    {
        ImGui::End();
    }
    operator bool() { return m_isOpened; }
};
class WindowChild
{
    bool m_isOpened = false;
public:
    WindowChild(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0)
    {
        m_isOpened = ImGui::BeginChild(str_id, size, border, flags);
    }
    ~WindowChild()
    {
        ImGui::EndChild();
    }
    operator bool() { return m_isOpened; }
};
class TabBar
{
    bool m_opened;
public:
    TabBar(const char* str_id, ImGuiTabBarFlags flags = 0)
    {
        m_opened = ImGui::BeginTabBar(str_id, flags);
    }
    ~TabBar()
    {
        if (m_opened) {
            ImGui::EndTabBar();
        }
    }
    operator bool() { return m_opened; }
};
class Tab
{
    bool m_opened;
public:
    Tab(const char* label, bool* p_open = NULL, ImGuiTabItemFlags flags = 0)
    {
        m_opened = ImGui::BeginTabItem(label, p_open, flags);
    }
    ~Tab()
    {
        if (m_opened) {
            ImGui::EndTabItem();
        }
    }
    operator bool() { return m_opened; }
};
class TreeNode
{
    bool m_opened;
public:
    TreeNode(const char* label)
    {
        m_opened = ImGui::TreeNode(label);
    }
    ~TreeNode()
    {
        if (m_opened) {
            ImGui::TreePop();
        }
    }
    operator bool() { return m_opened; }
};
class Popup
{
    bool m_opened;
public:
    Popup(const char* std_id, ImGuiWindowFlags flags = 0)
    {
        m_opened = ImGui::BeginPopup(std_id, flags);
    }
    ~Popup()
    {
        if (m_opened) {
            ImGui::EndPopup();
        }
    }
    operator bool() { return m_opened; }
};
class PopupModal
{
    bool m_opened;
public:
    PopupModal(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0)
    {
        m_opened = ImGui::BeginPopupModal(name, p_open, flags);
    }
    ~PopupModal()
    {
        if (m_opened) {
            ImGui::EndPopup();
        }
    }
    operator bool() { return m_opened; }
};
class Indent
{
    float m_indent;
public:
    Indent(float indent = 0.0f) : m_indent(indent)
    {
        ImGui::Indent(indent);
    }
    ~Indent()
    {
        ImGui::Unindent(m_indent);
    }
};
class MenuBar
{
    bool m_opened;
public:
    MenuBar()
    {
        m_opened = ImGui::BeginMenuBar();
    }
    ~MenuBar()
    {
        if (m_opened) {
            ImGui::EndMenuBar();
        }
    }
    operator bool() { return m_opened; }
};
class Menu
{
    bool m_opened;
public:
    Menu(const char* label, bool enabled = true)
    {
        m_opened = ImGui::BeginMenu(label, enabled);
    }
    ~Menu()
    {
        if (m_opened) {
            ImGui::EndMenu();
        }
    }
    operator bool() { return m_opened; }
};
class PushID
{
public:
    PushID(int int_id)
    {
        ImGui::PushID(int_id);
    }
    PushID(const void* ptr_id)
    {
        ImGui::PushID(ptr_id);
    }
    PushID(const char* str_id_begin, const char* str_id_end)
    {
        ImGui::PushID(str_id_begin, str_id_end);
    }
    PushID(const char* str_id)
    {
        ImGui::PushID(str_id);
    }
    ~PushID()
    {
        ImGui::PopID();
    }
};
}
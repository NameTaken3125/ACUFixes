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
}
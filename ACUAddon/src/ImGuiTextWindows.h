#pragma once

namespace ImGui
{

class ReadOnlyTextWindow
{
    std::string m_label;
    std::string m_outputBuffer;
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;
public:
    ReadOnlyTextWindow(const std::string_view& label) : m_label(label) { m_outputBuffer.reserve(1024); }
    void Draw()
    {
        ImGui::InputTextMultiline(m_label.c_str(), const_cast<char*>(m_outputBuffer.data()), m_outputBuffer.size(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
    }
    void SetOutput(const std::string_view& s)
    {
        m_outputBuffer = s;
    }
    void Append(const std::string_view& s)
    {
        m_outputBuffer += s;
    }
    void Clear()
    {
        m_outputBuffer.clear();
    }
};
class TextWindowInput
{
    std::string m_label;
public:
    std::string m_inputBuffer = std::string(1024, 0);
    TextWindowInput(const std::string_view& label) : m_label(label) { m_inputBuffer.reserve(1024); }
    bool Draw()
    {
        return ImGui::InputTextMultiline(m_label.c_str(), const_cast<char*>(m_inputBuffer.data()), m_inputBuffer.size(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
    }
};
}

inline std::string string_printf(const char* fmt, ...)
{
    std::string s{};
    va_list args, args2;
    va_start(args, fmt);
    va_copy(args2, args);

    s.resize(vsnprintf(nullptr, 0, fmt, args2) + 1);
    va_end(args2);
    vsprintf_s(s.data(), s.size(), fmt, args);
    va_end(args);
    s.pop_back();
    return s;
}
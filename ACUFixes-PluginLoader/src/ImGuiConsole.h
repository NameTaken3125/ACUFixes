#pragma once

enum class ConsoleMode
{
    Hidden = 0,
    ForegroundAndFocusable,
    BackgroundSemitransparentAndUnfocusable,
};
extern ConsoleMode g_ConsoleMode;
void ToggleConsoleMode();

// Adapted from ImGui Demo's "Example App: Debug Console"
struct ImGuiConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    using Mutex_t = std::mutex;
    Mutex_t m_Mutex;
    std::string m_SingleLineToCopy;

    ImGuiConsole();
    ~ImGuiConsole();

    // Portable helpers
    static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s) { size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog();
    void    AddLog(const char* s)
    {
        std::lock_guard _lock{ m_Mutex };
        Items.push_back(Strdup(s));
    }
    void    AddLogF(const char* fmt, ...) IM_FMTARGS(2)
    {
        va_list args;
        va_start(args, fmt);
        AddLogFV(fmt, args);
        va_end(args);
    }
    void    AddLogFV(const char* fmt, va_list args) IM_FMTARGS(2)
    {
        char buf[1024];
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf) - 1] = 0;
        AddLog(buf);
    }
    void    ExecCommand(const char* command_line);
    void    DrawIfVisible(const char* title, ConsoleMode consoleMode);

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
    int     TextEditCallback(ImGuiInputTextCallbackData* data);

    static ImGuiConsole& GetSingleton() { static ImGuiConsole singleton; return singleton; }
};

static void DrawConsoleIfVisible()
{
    ImGuiConsole::GetSingleton().DrawIfVisible("ACUFixes-ImGuiConsole", g_ConsoleMode);
}

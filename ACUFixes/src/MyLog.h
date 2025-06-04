#pragma once

#include <filesystem>
namespace fs = std::filesystem;

// The logging macro.

#define LOG_DEBUG(LoggerVariable, fmt, ...) (LoggerVariable).LogDebug(fmt, __VA_ARGS__)

// The macros for creation of typical additional loggers if you need them.

#define DEFINE_LOGGER_CONSOLE_AND_FILE(VariableName, textualName) Logger_ConsoleAndFile VariableName(textualName);
#define DEFINE_LOGGER_CONSOLE(VariableName, textualName) Logger_Console VariableName(textualName);
#define DEFINE_LOGGER_FILE(VariableName, textualName) Logger_File VariableName(textualName);
#define DEFINE_LOGGER_NULL(VariableName, textualName) Logger_Null VariableName;

/*
This logging is pretty basic.
1. Single LOG_DEBUG(LoggerVariable, fmt, ...) macro.
2. The logger is specified as parameter in the macro.
   The first idea was to skip specifying the logger:
        LOG_DEBUG("Something to log.");
   and implicitly utilize the __THE_LOGGER variable from the scope.
   But I feel like I want to support loggers provided as a function parameter,
   and because I'm wary of __THE_LOGGER name collisions if I decide to use many scoped
   loggers.
3. No standardized levels of logging, but the default ImGuiConsole does highlight a logged string in red
   if it contains "[error]". So you can do
        LOG_DEBUG(DefaultLogger, "[error] Everything crashed again.");
   or I guess maybe
        DEFINE_LOGGER_CONSOLE_AND_FILE(Error, DefaultLogger.m_name + "[error]");
        LOG_DEBUG(Error, "Everything crashed again.");




Usage examples:

a) Simplest usage (use the default logger name; output into both log file and ImGui Console):
#include "DefaultLogger.h"
void ExampleFunction()
{
    LOG_DEBUG(DefaultLogger, "Some info");
    LOG_DEBUG(DefaultLogger, "[error] Some error: %s.", errorDescription); // "[error]" is automatically colored red in the console.
}


b) Custom name for a logger for the scope (like for a specific .cpp file, a specific namespace or a function):
#include "Logging.h"
void ExampleFunction2()
{
    DEFINE_LOGGER_CONSOLE_AND_FILE(ScopedLogger, DefaultLogger.m_Name + "[A Custom Logger]");
    LOG_DEBUG(ScopedLogger, "Some info."); // -> "[YourPlugin.dll][A Custom Logger]Some info."
}

c) If you want to disable that one logger among many,
   you can instead do something like:

void ExampleFunction3()
{
#ifdef ENABLE_THIS_SPECIAL_LOGGER
#define DEFINE_THIS_SPECIAL_LOGGER(VariableName, textualName) DEFINE_LOGGER_CONSOLE_AND_FILE(VariableName, textualName)
#else
#define DEFINE_THIS_SPECIAL_LOGGER(VariableName, textualName) DEFINE_LOGGER_NULL(VariableName, textualName)
#endif
    DEFINE_THIS_SPECIAL_LOGGER(ScopedLogger, DefaultLogger.m_Name +"[A Custom Logger]");
    LOG_DEBUG(ScopedLogger, "Some info."); // -> logs if ENABLE_THIS_SPECIAL_LOGGER is defined, otherwise does nothing.
}
*/


extern FILE* g_LogFile;
class MyLogFileLifetime
{
public:
    MyLogFileLifetime(const fs::path& filepath);
    ~MyLogFileLifetime();
private:
    const fs::path& m_filepath;
};
#include "Common_Plugins/Console/ImGuiConsole.h"
#include "Serialization/UTF8.h"
struct Logger_Null
{
    static void LogDebug(const char* fmt, ...) {}
};
struct Logger_ConsoleAndFile
{
    const std::string m_Name;
    Logger_ConsoleAndFile(std::string_view name) : m_Name(name) {}
    void LogDebug(const char* fmt, ...)
    {
        // TODO: Reimplement ImGuiTextBuffer to start with a stack buffer of 1024 bytes.
        ImGuiTextBuffer buf;
        buf.reserve(1024);
        buf.append(m_Name.c_str());
        va_list args;
        va_start(args, fmt);
        buf.appendfv(fmt, args);
        va_end(args);
        ImGuiConsole::AddLog(buf.c_str());
        fprintf(g_LogFile, buf.c_str());
        fflush(g_LogFile);
    }
};
struct Logger_Console
{
    const std::string m_Name;
    Logger_Console(std::string_view name) : m_Name(name) {}
    void LogDebug(const char* fmt, ...)
    {
        ImGuiTextBuffer buf;
        buf.reserve(1024);
        buf.append(m_Name.c_str());
        va_list args;
        va_start(args, fmt);
        buf.appendfv(fmt, args);
        va_end(args);
        ImGuiConsole::AddLog(buf.c_str());
    }
};
struct Logger_File
{
    const std::string m_Name;
    Logger_File(std::string_view name) : m_Name(name) {}
    void LogDebug(const char* fmt, ...)
    {
        ImGuiTextBuffer buf;
        buf.reserve(1024);
        buf.append(m_Name.c_str());
        va_list args;
        va_start(args, fmt);
        buf.appendfv(fmt, args);
        va_end(args);
        fprintf(g_LogFile, buf.c_str());
        fflush(g_LogFile);
    }
};

inline DEFINE_LOGGER_CONSOLE_AND_FILE(DefaultLogger, "[" THIS_DLL_PROJECT_TARGET_FILE_NAME "]");

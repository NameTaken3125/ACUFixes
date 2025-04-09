#pragma once

#include <filesystem>
namespace fs = std::filesystem;

// The logging macro.

#define LOG_DEBUG(LoggerVariable, fmt, ...) (LoggerVariable).LogDebug(fmt, __VA_ARGS__)

// The macros for creation of typical additional loggers if you need them.

#define DEFINE_LOGGER_CONSOLE_AND_FILE(VariableName, textualName) Logger_ConsoleAndFile VariableName(textualName);
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
    void LogDebug(const wchar_t* fmt, ...)
    {
        // Append logger name to file.
        int filePrintReturn1 = fwprintf(g_LogFile, utf8_and_wide_string_conversion::utf8_decode(m_Name).c_str());
        // Append formatted string to file.
        va_list args;
        va_list argsCopy;
        va_start(args, fmt);
        va_copy(argsCopy, args);
        int filePrintReturn2 = vfwprintf(g_LogFile, fmt, args);
        fflush(g_LogFile);
        // Format string into stringbuffer.
        const int numWideChars = filePrintReturn1 + filePrintReturn2;
        wchar_t buf[1024];
        std::wstring heapBuf;
        wchar_t* fittingBuf = nullptr;
        constexpr int bufSize = IM_ARRAYSIZE(buf);
        if (numWideChars < bufSize)
        {
            _vsnwprintf_s(buf, IM_ARRAYSIZE(buf), fmt, argsCopy);
            buf[IM_ARRAYSIZE(buf) - 1] = 0;
            fittingBuf = buf;
        }
        else
        {
            heapBuf.resize(numWideChars + 1);
            _vsnwprintf_s(&heapBuf[0], numWideChars + 1, numWideChars, fmt, argsCopy);
            fittingBuf = &heapBuf[0];
        }
        va_end(argsCopy);
        va_end(args);

        std::string asUtf8 = utf8_and_wide_string_conversion::utf8_encode(fittingBuf);
        // Prepend logger name to formatted string.
        ImGuiConsole::AddLog((m_Name + asUtf8).c_str());
    }
};

inline DEFINE_LOGGER_CONSOLE_AND_FILE(DefaultLogger, "[" THIS_DLL_PROJECT_TARGET_FILE_NAME "]");

#include "pch.h"

#include "MyLog.h"

FILE* g_LogFile = nullptr;
void LOG_DEBUG_onlyFile(const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfwprintf(g_LogFile, fmt, args);
    fflush(g_LogFile);
    va_end(args);
}
MyLogFileLifetime::MyLogFileLifetime(const fs::path& filepath)
    : m_filepath(filepath)
{
    g_LogFile = _fsopen(filepath.string().c_str(), "w", _SH_DENYNO);
    if (!g_LogFile)
    {
        std::string er = "Can't open " + filepath.string() + "file.\n";
        puts(er.c_str());
        return;
    }
}
MyLogFileLifetime::~MyLogFileLifetime()
{
    if (g_LogFile)
    {
        fclose(g_LogFile);
        g_LogFile = nullptr;
    }
}

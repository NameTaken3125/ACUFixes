#include "pch.h"

#include "MyLog.h"

FILE* g_LogFile = nullptr;
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

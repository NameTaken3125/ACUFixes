#pragma once

#include <filesystem>
namespace fs = std::filesystem;



void LOG_DEBUG(const char* fmt, ...);



class MyLogFileLifetime
{
public:
    MyLogFileLifetime(const fs::path& filepath);
    ~MyLogFileLifetime();
private:
    const fs::path& m_filepath;
};

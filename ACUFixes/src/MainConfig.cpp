#include "pch.h"

#include "MyLog.h"

#include "MainConfig.h"
#include "SimpleJSON/json.hpp"
#include "Serialization/Serialization.h"
#include "Serialization/BooleanAdapter.h"
#include "Serialization/ToFromFile/ToFromFile.h"
#include "Serialization/UTF8.h"

ConfigTop g_Config;


//https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar/59617138#59617138
std::wstring ConvertAnsiToWide(const std::string& str)
{
    int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), &wstr[0], count);
    return wstr;
}
std::wstring asciiString2WString(const std::string& str)
{
    return ConvertAnsiToWide(str);
}
namespace MainConfig {
fs::path g_ConfigFilepath;
JSON ReadMainConfigFile()
{
    fs::path& configFullPath = g_ConfigFilepath;
    JSON cfg = json::FromFile(configFullPath);
    LOG_DEBUG(DefaultLogger, "Read from config file \"%s\":\n%s\n", configFullPath.u8string().c_str(),
        cfg.dump().c_str());
    return cfg;
}
void WriteToFile()
{
    JSON cfg;
    g_Config.SectionToJSON(cfg);
    fs::path& configFullPath = g_ConfigFilepath;
    LOG_DEBUG(DefaultLogger, "Writing to config file \"%s\":\n%s\n", configFullPath.u8string().c_str(),
        cfg.dump().c_str());
    json::ToFile(cfg, configFullPath);
}
void FindAndLoadConfigFileOrCreateDefault(const fs::path& filename)
{
    g_ConfigFilepath = filename;
    JSON cfg = ReadMainConfigFile();
    g_Config.SectionFromJSON(cfg);
    WriteToFile();
}
} // namespace MainConfig

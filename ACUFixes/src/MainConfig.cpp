#include "pch.h"

#include "MyLog.h"

#include "MainConfig.h"
#include "SimpleJSON/json.hpp"
#include "Serialization/Serialization.h"
#include "Serialization/BooleanAdapter.h"
#include "Serialization/ToFromFile/ToFromFile.h"

ConfigTop g_Config;


namespace MainConfig {
fs::path g_ConfigFilepath;
JSON ReadMainConfigFile()
{
    fs::path& configFullPath = g_ConfigFilepath;
    JSON cfg = json::FromFile(configFullPath);
    LOG_DEBUG("Read from config file \"%s\":\n%s\n", configFullPath.string().c_str(), cfg.dump().c_str());
    return cfg;
}
void WriteToFile()
{
    JSON cfg;
    g_Config.SectionToJSON(cfg);
    fs::path& configFullPath = g_ConfigFilepath;
    LOG_DEBUG("Writing to config file \"%s\":\n%s\n", configFullPath.string().c_str(), cfg.dump().c_str());
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

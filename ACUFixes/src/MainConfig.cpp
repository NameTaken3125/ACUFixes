#include "pch.h"

#include "MyLog.h"

// I'm not sure if the JSON library can properly handle file encodings,
// especially if the config files get edited by hand.
// But I don't want to clutter the ImGui Console, and so this only goes into the file.
DEFINE_LOGGER_FILE(ConfigLog, DefaultLogger.m_Name + "[Config]")

#include "MainConfig.h"
#include "SimpleJSON/json.hpp"
#include "Serialization/Serialization.h"
#include "Serialization/BooleanAdapter.h"
#include "Serialization/ToFromFile/ToFromFile.h"
#include "Serialization/UTF8.h"

ConfigTop g_Config;


namespace MainConfig {
fs::path g_ConfigFilepath;
JSON ReadMainConfigFile()
{
    fs::path& configFullPath = g_ConfigFilepath;
    JSON cfg = json::FromFile(configFullPath);
    LOG_DEBUG(ConfigLog, "Read from config file \"%s\":\n%s\n", configFullPath.u8string().c_str(),
        cfg.dump().c_str());
    return cfg;
}
void WriteToFile()
{
    JSON cfg;
    g_Config.SectionToJSON(cfg);
    fs::path& configFullPath = g_ConfigFilepath;
    LOG_DEBUG(ConfigLog, "Writing to config file \"%s\":\n%s\n", configFullPath.u8string().c_str(),
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

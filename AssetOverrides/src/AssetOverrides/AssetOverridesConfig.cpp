#include "pch.h"

#include "MyLog.h"

#include "AssetOverridesConfig.h"
#include "SimpleJSON/json.hpp"
#include "Serialization/Serialization.h"
#include "Serialization/BooleanAdapter.h"
#include "Serialization/ToFromFile/ToFromFile.h"
#include "Serialization/UTF8.h"

AssetOverridesConfigTop g_AssetOverridesConfig;

namespace AssetOverridesConfig {
fs::path g_AssetOverridesConfigFilepath;
JSON ReadMainConfigFile()
{
    fs::path& configFullPath = g_AssetOverridesConfigFilepath;
    JSON cfg = json::FromFile(configFullPath);
    LOG_DEBUG(DefaultLogger, "Read from config file \"%s\":\n%s\n", configFullPath.u8string().c_str(),
        cfg.dump().c_str());
    return cfg;
}
void WriteToFile()
{
    JSON cfg;
    g_AssetOverridesConfig.SectionToJSON(cfg);
    fs::path& configFullPath = g_AssetOverridesConfigFilepath;
    LOG_DEBUG(DefaultLogger, "Writing to config file \"%s\":\n%s\n", configFullPath.u8string().c_str(),
        cfg.dump().c_str());
    if (!json::ToFile(cfg, configFullPath))
    {
        LOG_DEBUG(DefaultLogger,
            "[error]Failed to write to %s\n"
            , configFullPath.u8string().c_str()
        );
    }
}
void FindAndLoadConfigFileOrCreateDefault(const fs::path& filename)
{
    g_AssetOverridesConfigFilepath = filename;
    JSON cfg = ReadMainConfigFile();
    g_AssetOverridesConfig.SectionFromJSON(cfg);
    WriteToFile();
}
} // namespace AssetOverridesConfig

#include "pch.h"

#include "MyLog.h"

#include "PluginLoaderConfig.h"
#include "SimpleJSON/json.hpp"
#include "Serialization/Serialization.h"
#include "Serialization/BooleanAdapter.h"
#include "Serialization/ToFromFile/ToFromFile.h"

#include "base.h"

const char* g_PluginLoaderConfig_Filename = "ACUFixes-PluginLoader-config.json";
ConfigTop g_PluginLoaderConfig;

fs::path& GetThisDLLAbsolutePath()
{
    static fs::path dllPath = []()
    {
        char path[MAX_PATH];
        GetModuleFileNameA(Base::Data::thisDLLModule, path, (DWORD)std::size(path));
        return fs::path(path);
    }();
    return dllPath;
}
fs::path AbsolutePathInMyDirectory(const fs::path& filenameRel)
{
    fs::path& dll = GetThisDLLAbsolutePath();
    fs::path fullPath = dll.parent_path() / filenameRel;
    return fullPath;
}

namespace PluginLoaderConfig {
fs::path GetMainConfigFilepath()
{
    return AbsolutePathInMyDirectory(g_PluginLoaderConfig_Filename);
}
JSON ReadMainConfigFile()
{
    fs::path configFullPath = GetMainConfigFilepath();
    JSON cfg = json::FromFile(configFullPath);
    LOG_DEBUG("Read from config file \"%s\":\n%s\n", configFullPath.string().c_str(), cfg.dump().c_str());
    return cfg;
}
void WriteToFile()
{
    JSON cfg;
    g_PluginLoaderConfig.SectionToJSON(cfg);
    fs::path configFullPath = GetMainConfigFilepath();
    LOG_DEBUG("Writing to config file \"%s\":\n%s\n", configFullPath.string().c_str(), cfg.dump().c_str());
    json::ToFile(cfg, configFullPath);
}
} // namespace PluginLoaderConfig
void PluginLoaderConfig::FindAndLoadConfigFileOrCreateDefault()
{
    JSON cfg = ReadMainConfigFile();
    g_PluginLoaderConfig.SectionFromJSON(cfg);
    PluginLoaderConfig::WriteToFile();
}

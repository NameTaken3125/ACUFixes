#include "pch.h"

#include "MyLog.h"

#include "MainConfig.h"
#include "SimpleJSON/json.hpp"
#include "Serialization/Serialization.h"
#include "Serialization/BooleanAdapter.h"
#include "Serialization/ToFromFile/ToFromFile.h"

#include "base.h"

ConfigTop g_Config;

fs::path& GetThisDLLAbsolutePath()
{
    static fs::path dllPath = []()
    {
        char path[MAX_PATH];
        GetModuleFileName(Base::Data::thisDLLModule, path, (DWORD)std::size(path));
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

namespace MainConfig {
fs::path GetMainConfigFilepath()
{
    std::string configFileName = "acufixes-config.json";
    return AbsolutePathInMyDirectory(configFileName);
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
    g_Config.SectionToJSON(cfg);
    fs::path configFullPath = GetMainConfigFilepath();
    LOG_DEBUG("Writing to config file \"%s\":\n%s\n", configFullPath.string().c_str(), cfg.dump().c_str());
    json::ToFile(cfg, configFullPath);
}
} // namespace MainConfig
void MainConfig::FindAndLoadConfigFileOrCreateDefault()
{
    JSON cfg = ReadMainConfigFile();
    g_Config.SectionFromJSON(cfg);
    MainConfig::WriteToFile();
}

#include "pch.h"

#include "MyLog.h"

#include "MainConfig.h"
#include "SimpleJSON/json.hpp"
#include "Serialization/Serialization.h"
#include "Serialization/BooleanAdapter.h"

#include "base.h"

namespace json {
JSON FromFile(const fs::path& path)
{
    std::ifstream ifs(path);
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string loadedString = ss.str();
    return JSON::Load(loadedString);
}
void ToFile(const JSON& obj, const fs::path& path)
{
    std::ofstream ofs(path);
    ofs << obj.dump();
}
} // namespace json


namespace MainConfig {

bool imgui_useImGui = true;
bool imgui_showSuccessfulInjectionIndicator = true;

} // namespace MainConfig


using json::JSON;
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

void DumpConfig(JSON& cfg)
{
    using namespace MainConfig;
    WRITE_JSON_VARIABLE(cfg, imgui_showSuccessfulInjectionIndicator, BooleanAdapter);
    WRITE_JSON_VARIABLE(cfg, imgui_useImGui, BooleanAdapter);
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
JSON& GetConfigJSON()
{
    static JSON configFileJSON = ReadMainConfigFile();
    return configFileJSON;
}
void WriteToFile()
{
    JSON& cfg = MainConfig::GetConfigJSON();
    fs::path configFullPath = GetMainConfigFilepath();
    LOG_DEBUG("Writing to config file \"%s\":\n%s\n", configFullPath.string().c_str(), cfg.dump().c_str());
    json::ToFile(cfg, configFullPath);
}
} // namespace MainConfig
void MainConfig::FindAndLoadConfigFileOrCreateDefault()
{
    JSON& cfg = MainConfig::GetConfigJSON();
    if (cfg.IsObject())
    {
        READ_JSON_VARIABLE(cfg, imgui_showSuccessfulInjectionIndicator, BooleanAdapter);
        READ_JSON_VARIABLE(cfg, imgui_useImGui, BooleanAdapter);
    }
    DumpConfig(cfg);
    MainConfig::WriteToFile();
}

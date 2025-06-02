#include "pch.h"

#include "Serialization/ToFromFile/ToFromFile.h"

namespace json {
JSON FromFile(const fs::path& path)
{
    std::ifstream ifs(path);
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string loadedString = ss.str();
    return JSON::Load(loadedString);
}
bool ToFile(const JSON& obj, const fs::path& path)
{
    try
    {
        fs::create_directories(path.parent_path());
        std::ofstream ofs(path);
        ofs << obj.dump();
        return true;
    }
    catch (const fs::filesystem_error&)
    {
        // fs::create_directories() can throw e.g. if trying to create a directory
        // that actually already corresponds to a symlink that lost its target.
        return false;
    }
}
}

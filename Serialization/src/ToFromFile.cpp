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
void ToFile(const JSON& obj, const fs::path& path)
{
    std::ofstream ofs(path);
    ofs << obj.dump();
}
}

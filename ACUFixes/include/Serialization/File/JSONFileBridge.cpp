#include "JSONFileBridge.h"


#include <fstream>
#include <sstream>


std::string ReadTextFileOrEmptyString(const fs::path& filepath)
{
    std::ifstream ifs(filepath);
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}
JSONFileBridge::JSONFileBridge(const fs::path& filepath, bool shouldWriteOnDestruction)
    : m_filepath(filepath)
    , m_shouldWriteOnDestruction(shouldWriteOnDestruction)
    , m_jsonObject(json::JSON::Load(ReadTextFileOrEmptyString(filepath)))
{}
JSONFileBridge::~JSONFileBridge()
{
    if (m_shouldWriteOnDestruction)
        WriteNow();
}
void JSONFileBridge::WriteNow()
{
    std::ofstream ofs(m_filepath);
    ofs << m_jsonObject.dump();
}
void JSONFileBridge::DontWrite()
{
    m_shouldWriteOnDestruction = false;
}

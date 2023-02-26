#pragma once

#include <filesystem>
#include "SimpleJSON/json.hpp"
namespace fs = std::filesystem;

// Reads JSON from file on creation, optionally dumps to the same file on destruction.
class JSONFileBridge
{
public:
    json::JSON m_jsonObject;
    JSONFileBridge(const fs::path& filepath, bool shouldWriteOnDestruction);
    // Write manually.
    void WriteNow();
    // Won't write on destruction.
    void DontWrite();
public:
    ~JSONFileBridge();
private:
    fs::path m_filepath;
    bool m_shouldWriteOnDestruction;
};

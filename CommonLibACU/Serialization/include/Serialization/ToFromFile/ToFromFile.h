#pragma once

#include <fstream>
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;
#include "SimpleJSON/json.hpp"

namespace json {

JSON FromFile(const fs::path& path);
bool ToFile(const JSON& obj, const fs::path& path);

}

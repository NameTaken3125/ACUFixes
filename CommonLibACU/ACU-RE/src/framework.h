#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <intrin.h>
#include <string_view>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
#include <optional>

#include "ACU/basic_types.h"
#include "vmath/vmath.h"

#pragma once

#include <filesystem>
namespace fs = std::filesystem;
#include <Windows.h>
#include "Serialization/Serialization.h"
#include "Serialization/UTF8.h"

namespace filepath_string_conversion {
    using utf8_and_wide_string_conversion::utf8_encode;
    using utf8_and_wide_string_conversion::utf8_decode;
} // namespace filepath_string_conversion

struct FilepathAdapter : public JSONAdapter<fs::path, JSON::Class::String>
{
    using BaseAdapter::BaseAdapter;
    // Serialize.
    JSON ToJSON()
    {
        return filepath_string_conversion::utf8_encode(source.native());
    }
    // Deserialize.
    bool FromJSON(const JSON& obj)
    {
        if (!this->IsCorrectJSONType(obj))
        {
            return false;
        }
        source = filepath_string_conversion::utf8_decode(obj.ToStringNoEscape());
        return true;
    }
};

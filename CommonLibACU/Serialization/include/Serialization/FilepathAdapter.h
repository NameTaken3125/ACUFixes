#pragma once

#include <filesystem>
namespace fs = std::filesystem;
#include <Windows.h>
#include "Serialization/Serialization.h"

// Thanks, Jamerson https://codereview.stackexchange.com/questions/419/converting-between-stdwstring-and-stdstring/146738#146738
namespace filepath_string_conversion {
// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string& str)
{
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
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

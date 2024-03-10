#pragma once

#include <array>
#include "Common_Plugins/Common_PluginSide.h"

/*
The game uses 8-byte Handles to identify its data resources.
This namespaces provides a function that can convert a handle into the corresponding filename.
This utilizes an external "handles map" file.
*/
namespace ACU::Handles
{
class HandleString
{
public:
    std::array<char, 0x100> m_buffer = { 0 };
    operator const char* () { return m_buffer.data(); }
    const char* c_str() const { return m_buffer.data(); }
    size_t size() const { return strnlen_s(m_buffer.data(), m_buffer.size()); }
    bool empty() const { return this->size() == 0; }
};
HandleString HandleToText(uint64 handle);
}

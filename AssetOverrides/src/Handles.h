#pragma once

#include <array>
#include "Common_Plugins/Common_PluginSide.h"

/*
The game uses 8-byte Handles to identify its data resources.
This namespace provides a function that can convert a handle into the corresponding filename.
This utilizes an external "dictionary of handles" file. The ACUFixes Github page should contain
this "HandlesLUT-ACUnity.handlesmapcpp" file, maybe somewhere in the Releases files.
It needs to be placed in the folder with the plugin DLL:
    Assassin's Creed Unity/ACUFixes/plugins/HandlesLUT-ACUnity.handlesmapcpp
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



// Try to load the "dictionary of handles".
// You don't have to call this function, it will be called the first time you use
// ACU::Handles::HandleToText().
void LoadHandlesmapFile();
}

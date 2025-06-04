#pragma once

#include "ACU/basic_types.h"

/*
The game uses 8-byte Handles to identify its data resources.
This namespace provides a function that finds a filename corresponding to a given handle.
This utilizes an external "dictionary of handles" file. The ACUFixes Github page should contain
this "HandlesLUT-ACUnity.handlesmapcpp" file, maybe somewhere in the Releases files.
It needs to be placed in the folder with the plugin DLL:
    Assassin's Creed Unity/ACUFixes/plugins/HandlesLUT-ACUnity.handlesmapcpp
*/
namespace ACU::Handles
{

const char* HandleToText(uint64 handle);


// Try to load the "dictionary of handles".
// You don't have to call this function, it will be called the first time you use
// ACU::Handles::HandleToText().
void LoadHandlesmapFile();
}

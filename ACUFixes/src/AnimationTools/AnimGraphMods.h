#pragma once

#include "ACU/basic_types.h"

class AtomGraph;

void LetsDoSomePlayerGraphMods(AtomGraph& atomGraph);
struct MyNewRTCPVariable
{
    const char* strtype = "bool";
    const char* varname = "MyNewBoolGraphVariable";
    uint32 varnameHash = 91818771;
    bool InitialValue = 0;
};
extern MyNewRTCPVariable g_newGraphVar;

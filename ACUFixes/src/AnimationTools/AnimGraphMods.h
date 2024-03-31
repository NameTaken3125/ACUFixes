#pragma once

#include "ACU/basic_types.h"

class AtomGraph;

void LetsDoSomePlayerGraphMods(AtomGraph& atomGraph);
enum class RTCPVariableType
{
    Bool = 0,
    Float = 1,
};
struct MyNewerRTCPVariable
{
    const char* varname;
    uint32 varnameHash;
    const RTCPVariableType vartype;
    union
    {
        bool bool_;
        float float_;
    };
    MyNewerRTCPVariable(const char* varname, uint32 varnameHash, bool bool_)
        : varname(varname), varnameHash(varnameHash), bool_(bool_), vartype(RTCPVariableType::Bool)
    {}
    MyNewerRTCPVariable(const char* varname, uint32 varnameHash, float float_)
        : varname(varname), varnameHash(varnameHash), float_(float_), vartype(RTCPVariableType::Float)
    {}
};
extern MyNewerRTCPVariable g_newGraphVar;

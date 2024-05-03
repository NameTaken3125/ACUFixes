#pragma once

#include "ACU/basic_types.h"

enum class RTCPVariableType
{
    Bool = 0,
    Float = 1,
    Int = 2,
};
class RTCPVariableDescriptor
{
public:
    const char* varname;
    uint32 varnameHash;
    const RTCPVariableType vartype;
    union
    {
        bool bool_;
        float float_;
        int int_;
    };
    explicit RTCPVariableDescriptor(const char* varname, uint32 varnameHash, bool bool_)
        : varname(varname), varnameHash(varnameHash), bool_(bool_), vartype(RTCPVariableType::Bool)
    {}
    explicit RTCPVariableDescriptor(const char* varname, uint32 varnameHash, float float_)
        : varname(varname), varnameHash(varnameHash), float_(float_), vartype(RTCPVariableType::Float)
    {}
    explicit RTCPVariableDescriptor(const char* varname, uint32 varnameHash, int int_)
        : varname(varname), varnameHash(varnameHash), int_(int_), vartype(RTCPVariableType::Int)
    {}
};

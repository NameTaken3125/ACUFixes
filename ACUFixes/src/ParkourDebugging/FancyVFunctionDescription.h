#pragma once

#include "ACU/basic_types.h"
#include "ACU/FancyVTable.h"

template<typename FunctionPtrType_, uint32 staticIdx>
struct FancyVFunctionDescription
{
    using FunctionPtrType = FunctionPtrType_;
    enum { idx = staticIdx };
};

#define GET_AND_CAST_FANCY_FUNC(obj, fancyFuncDescription) ((fancyFuncDescription::FunctionPtrType)((obj).fancyVTable->fancyVirtuals[fancyFuncDescription::idx].fn))

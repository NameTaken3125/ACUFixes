#pragma once


#define DEFINE_GAME_FUNCTION(FuncName, absoluteAddress, returnType, callingConvention, allParamsInParentheses) \
using FuncName##_t = returnType(callingConvention*)allParamsInParentheses;\
FuncName##_t FuncName = (FuncName##_t)absoluteAddress;

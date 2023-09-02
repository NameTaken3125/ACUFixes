#pragma once


#define DEFINE_GAME_FUNCTION(FuncName, absoluteAddress, returnType, callingConvention, allParamsInParentheses) \
static returnType(callingConvention* FuncName)allParamsInParentheses = (returnType(callingConvention*)allParamsInParentheses)absoluteAddress;

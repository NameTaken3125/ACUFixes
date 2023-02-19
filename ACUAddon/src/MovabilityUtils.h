#pragma once

#define MOVABLE_NOT_COPYABLE(Cls)\
Cls(const Cls& rhs) = delete;\
Cls(Cls&& rhs) = default;\
Cls& operator=(const Cls & rhs) = delete;\
Cls& operator=(Cls && rhs) = default;

#define NON_MOVABLE(Cls)\
Cls(const Cls& rhs) = delete;\
Cls(Cls&& rhs) = delete;\
Cls& operator=(const Cls & rhs) = delete;\
Cls& operator=(Cls && rhs) = delete;

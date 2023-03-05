#pragma once

/*
Thank you Suma https://stackoverflow.com/questions/147267/easy-way-to-use-variables-of-enum-types-as-string-in-c/202511#202511
*/

#include <string_view>

template<typename EnumType>
struct enum_reflection {
    static constexpr const char* GetString(EnumType value);
    static constexpr EnumType GetValue(const std::string_view& str);
};

constexpr const std::string_view INVALID_ENUM_TO_STRING = "INVALID_TO_STRING";

// expansion macro for enum value definition
#define ENUM_VALUE(name,assign) name assign,

// expansion macro for enum to string conversion
#define ENUM_CASE(name,assign) case current_enum_type::name: return #name;

// expansion macro for string to enum conversion
#define ENUM_STRCMP(name,assign) if (str == #name) return current_enum_type::name;

/// declare the access function and define enum values
#define DECLARE_ENUM(EnumType,ENUM_DEF) \
  enum class EnumType { \
    ENUM_DEF(ENUM_VALUE) \
  }; \
  template<> \
  constexpr const char *enum_reflection<EnumType>::GetString(EnumType value) \
  { \
    using current_enum_type = EnumType; \
    switch(value) \
    { \
      ENUM_DEF(ENUM_CASE) \
      default: return INVALID_ENUM_TO_STRING.data(); /* handle input error */ \
    } \
  } \
  template<> \
  constexpr EnumType enum_reflection<EnumType>::GetValue(const std::string_view& str) \
  { \
    using current_enum_type = EnumType; \
    ENUM_DEF(ENUM_STRCMP) \
    return EnumType::INVALID_FROM_STRING; /* handle input error */ \
  }
#pragma once

#include <optional>

/*
Thank you Suma https://stackoverflow.com/questions/147267/easy-way-to-use-variables-of-enum-types-as-string-in-c/202511#202511
Something to note:
    enum_reflection::GetString() uses a switch statement, meaning that the enum
    cannot have repeated values. This might be solved by replacing the `switch/case` statements
    with
        if (false)
        else if(...)
        else if(...)
        ...
    Or by using a map, if it comes to that.
*/

#include <string_view>
#include <array>

template<typename EnumType>
struct enum_reflection {
    static constexpr const char* GetString(EnumType value);
    static constexpr std::optional<EnumType> GetValue(const std::string_view& str);
    static constexpr size_t GetNumItems();
    static constexpr auto GetAllStrings();
    static constexpr auto GetAllValues();
    static constexpr auto GetAllPairs();
};

constexpr const std::string_view INVALID_ENUM_TO_STRING = "INVALID_TO_STRING";

// expansion macro for enum value definition
#define ENUM_VALUE(name,assign) name assign,

// expansion macro for enum to string conversion
#define ENUM_CASE(name,assign) case current_enum_type::name: return #name;

// expansion macro for string to enum conversion
#define ENUM_STRCMP(name,assign) if (str == #name) return current_enum_type::name;

// expansion macro for counting the number of elements
#define ENUM_COUNTER(name,assign) "1"

#define ENUM_STRING_AND_COMMA(name,assign) #name,
#define ENUM_VALUE_AND_COMMA(name,assign) current_enum_type::name,
#define ENUM_PAIR_AND_COMMA(name,assign) Pair{ current_enum_type::name, #name },

// Expand the macro-fied enum in several different ways, to define the enum class itself
// and various access functions.
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
  constexpr std::optional<EnumType> enum_reflection<EnumType>::GetValue(const std::string_view& str) \
  { \
    using current_enum_type = EnumType; \
    ENUM_DEF(ENUM_STRCMP) \
    return {}; /* handle input error */ \
  } \
  template<> \
  constexpr size_t enum_reflection<EnumType>::GetNumItems() \
  { \
    constexpr std::string_view one_char_for_every_element = ENUM_DEF(ENUM_COUNTER); \
    return one_char_for_every_element.size(); \
  } \
  template<> \
  constexpr auto enum_reflection<EnumType>::GetAllStrings() \
  { \
    constexpr std::array<const char*, GetNumItems()> result = { ENUM_DEF(ENUM_STRING_AND_COMMA) }; \
    return result; \
  } \
  template<> \
  constexpr auto enum_reflection<EnumType>::GetAllValues() \
  { \
    using current_enum_type = EnumType; \
    constexpr std::array<EnumType, GetNumItems()> result = { ENUM_DEF(ENUM_VALUE_AND_COMMA) }; \
    return result; \
  } \
  template<> \
  constexpr auto enum_reflection<EnumType>::GetAllPairs() \
  { \
    struct Pair { \
      EnumType value; const char* name; \
    }; \
    using current_enum_type = EnumType; \
    constexpr std::array<Pair, GetNumItems()> result = { ENUM_DEF(ENUM_PAIR_AND_COMMA) }; \
    return result; \
  }
